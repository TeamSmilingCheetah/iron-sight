#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"
#include "CLevel.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : CComponent(_Type)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin)
    : CComponent(_Origin)
      , m_Mesh(_Origin.m_Mesh)
{
    m_vecMtrls.resize(_Origin.m_vecMtrls.size());

    for (size_t i = 0; i < _Origin.m_vecMtrls.size(); ++i)
    {
        m_vecMtrls[i].pCurMtrl = _Origin.m_vecMtrls[i].pCurMtrl;
        m_vecMtrls[i].pSharedMtrl = _Origin.m_vecMtrls[i].pSharedMtrl;

        // ���� ������Ʈ�� ���������� �����ϰ� �ְ�, ���� ��������� ���������� �ƴѰ��
        if (_Origin.m_vecMtrls[i].pSharedMtrl != _Origin.m_vecMtrls[i].pCurMtrl)
        {
            assert(_Origin.m_vecMtrls[i].pDynamicMtrl.Get());

            // ���� ���� ������Ʈ�� ������ ���������� �����Ѵ�.
            GetDynamicMaterial(i);

            // ���� ����������Ʈ�� �������� ���� ���� ������ ���������� �����Ѵ�.
            *m_vecMtrls[i].pDynamicMtrl.Get() = *_Origin.m_vecMtrls[i].pDynamicMtrl.Get();
        }
        else
        {
            m_vecMtrls[i].pCurMtrl = m_vecMtrls[i].pSharedMtrl;
        }
    }
}

CRenderComponent::~CRenderComponent()
{
}


void CRenderComponent::SetMesh(Ptr<CMesh> _Mesh)
{
    m_Mesh = _Mesh;

    if (!m_vecMtrls.empty())
    {
        m_vecMtrls.clear();
        vector<tMtrlSet> vecMtrls;
        m_vecMtrls.swap(vecMtrls);
    }

    if (nullptr != m_Mesh)
        m_vecMtrls.resize(m_Mesh->GetSubsetCount());
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx)
{
    // ������ ����Ǹ� ������ ���纻 �޾Ƶ� DynamicMaterial �� �����Ѵ�.
    m_vecMtrls[_idx].pSharedMtrl = _Mtrl;
    m_vecMtrls[_idx].pCurMtrl = _Mtrl;
    m_vecMtrls[_idx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT _idx)
{
    return m_vecMtrls[_idx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _idx)
{
    // ���������� �������°����� ���� ��������� ������������ ȸ���ϵ��� �Ѵ�
    m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pSharedMtrl;

    if (m_vecMtrls[_idx].pDynamicMtrl.Get())
    {
        m_vecMtrls[_idx].pDynamicMtrl = nullptr;
    }

    return m_vecMtrls[_idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _idx)
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
        return nullptr;

    // ���� ������ ���� -> Nullptr ��ȯ
    if (nullptr == m_vecMtrls[_idx].pSharedMtrl)
    {
        m_vecMtrls[_idx].pCurMtrl = nullptr;
        m_vecMtrls[_idx].pDynamicMtrl = nullptr;
        return m_vecMtrls[_idx].pCurMtrl;
    }

    if (nullptr == m_vecMtrls[_idx].pDynamicMtrl)
    {
        m_vecMtrls[_idx].pDynamicMtrl = m_vecMtrls[_idx].pSharedMtrl->Clone();
        m_vecMtrls[_idx].pDynamicMtrl->SetName(m_vecMtrls[_idx].pSharedMtrl->GetName() + L"_Clone");
        m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pDynamicMtrl;
    }

    return m_vecMtrls[_idx].pCurMtrl;
}

void CRenderComponent::SaveComponent(FILE* _File)
{
    // �޽� �������� ����
    SaveAssetRef(m_Mesh, _File);

    // ���� �������� ����
    UINT iMtrlCount = GetMaterialCount();
    fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        SaveAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
    }
}

void CRenderComponent::LoadComponent(FILE* _File)
{
    // �޽� �������� �ҷ�����
    LoadAssetRef(m_Mesh, _File);

    // ���� �������� �ҷ�����
    UINT iMtrlCount = GetMaterialCount();
    fread(&iMtrlCount, sizeof(UINT), 1, _File);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        LoadAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
    }
}

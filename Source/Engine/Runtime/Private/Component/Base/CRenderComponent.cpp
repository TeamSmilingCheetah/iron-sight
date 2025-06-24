#include "pch.h"
#include "Runtime/Public/Component/Base/CRenderComponent.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"

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

        // 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
        if (_Origin.m_vecMtrls[i].pSharedMtrl != _Origin.m_vecMtrls[i].pCurMtrl)
        {
            assert(_Origin.m_vecMtrls[i].pDynamicMtrl.Get());

            // 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
            GetDynamicMaterial(static_cast<UINT>(i));

            // 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
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

void CRenderComponent::Render(UINT _iSubset)
{
	Render();
}

ULONG64 CRenderComponent::GetInstID(UINT _iMtrlIdx)
{
	if (m_Mesh == nullptr || m_vecMtrls[_iMtrlIdx].pCurMtrl == nullptr)
		return 0;

	uInstID id{ (UINT)m_Mesh->GetID(), (WORD)m_vecMtrls[_iMtrlIdx].pCurMtrl->GetID(), (WORD)_iMtrlIdx };
	return id.llID;
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
    // 재질이 변경되면 기존에 복사본 받아둔 DynamicMaterial 을 삭제한다.
    m_vecMtrls[_idx].pSharedMtrl = _Mtrl;
    m_vecMtrls[_idx].pCurMtrl = _Mtrl;
    m_vecMtrls[_idx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT _idx)
{
	// 가져오는 조건추가, 가진 재질수가0이거나 이상한 번호를 가져오면 nullptr반환
	if (m_vecMtrls.size() == 0 || m_vecMtrls.size() < _idx)
		return nullptr;

    return m_vecMtrls[_idx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _idx)
{
    // 공유재질을 가져오는것으로 현재 사용재질을 동적재질에서 회복하도록 한다
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

    // 원본 재질이 없다 -> Nullptr 반환
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
    // 메쉬 참조정보 저장
    SaveAssetRef(m_Mesh, _File);

    // 재질 참조정보 저장
    UINT iMtrlCount = GetMaterialCount();
    fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        SaveAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
    }
}

void CRenderComponent::LoadComponent(FILE* _File)
{
    // 메쉬 참조정보 불러오기
    LoadAssetRef(m_Mesh, _File);

    // 재질 참조정보 불러오기
    UINT iMtrlCount = GetMaterialCount();
    fread(&iMtrlCount, sizeof(UINT), 1, _File);

	m_vecMtrls.resize(iMtrlCount);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        LoadAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
		SetMaterial(m_vecMtrls[i].pSharedMtrl, i);
    }
}

#include "pch.h"
#include "CAnimator3D.h"

#include "CAssetMgr.h"
#include "CTimeMgr.h"
#include "CStructuredBuffer.h"
#include "components.h"

#include "CBoneMatrixCS.h"

CAnimator3D::CAnimator3D()
    : CComponent(COMPONENT_TYPE::ANIMATOR3D)
      , m_vecBones(nullptr)
      , m_vecClip(nullptr)
      , m_FrameCount(30)
      , m_CurTime(0.)
      , m_CurClip(0)
      , m_FrameIdx(0)
      , m_NextFrameIdx(0)
      , m_Ratio(0.f)
      , m_BoneFinalMatBuffer(nullptr)
      , m_bFinalMatUpdate(false)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
    : CComponent(COMPONENT_TYPE::ANIMATOR3D)
      , m_vecBones(_origin.m_vecBones)
      , m_vecClip(_origin.m_vecClip)
      , m_FrameCount(_origin.m_FrameCount)
      , m_CurTime(_origin.m_CurTime)
      , m_CurClip(_origin.m_CurClip)
      , m_FrameIdx(_origin.m_FrameIdx)
      , m_NextFrameIdx(_origin.m_NextFrameIdx)
      , m_Ratio(_origin.m_Ratio)
      , m_BoneFinalMatBuffer(nullptr)
      , m_bFinalMatUpdate(false)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
    if (nullptr != m_BoneFinalMatBuffer)
        delete m_BoneFinalMatBuffer;
}

void CAnimator3D::FinalTick()
{
    m_CurTime = 0.f;
    // ���� ������� Clip �� �ð��� �����Ѵ�.
    m_vecClipUpdateTime[m_CurClip] += EngineDT;

    if (m_vecClipUpdateTime[m_CurClip] >= m_vecClip->at(m_CurClip).dTimeLength)
    {
        m_vecClipUpdateTime[m_CurClip] = 0.f;
    }

    m_CurTime = m_vecClip->at(m_CurClip).dStartTime + m_vecClipUpdateTime[m_CurClip];

    // ���� ������ �ε��� ���ϱ�
    double dFrameIdx = m_CurTime * static_cast<double>(m_FrameCount);
    m_FrameIdx = static_cast<int>(dFrameIdx);

    // ���� ������ �ε���
    if (m_FrameIdx >= m_vecClip->at(0).iFrameLength - 1)
        m_NextFrameIdx = m_FrameIdx; // ���̸� ���� �ε����� ����
    else
        m_NextFrameIdx = m_FrameIdx + 1;

    // �����Ӱ��� �ð��� ���� ������ �����ش�.
    m_Ratio = static_cast<float>(dFrameIdx - (double)m_FrameIdx);

    // ��ǻƮ ���̴� ���꿩��
    m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
    m_vecClip = _vecAnimClip;
    m_vecClipUpdateTime.resize(m_vecClip->size());

    // �׽�Ʈ �ڵ�
    /*static float fTime = 0.f;
    fTime += 1.f;
    m_vecClipUpdateTime[0] = fTime;*/
}


void CAnimator3D::Binding()
{
    if (!m_bFinalMatUpdate)
    {
        // Animation3D Update Compute Shader
        static Ptr<CBoneMatrixCS> pBoneMatCS = new CBoneMatrixCS;

        // Bone Data
        Ptr<CMesh> pMesh = MeshRender()->GetMesh();
        check_mesh(pMesh);

        pBoneMatCS->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
        pBoneMatCS->SetOffsetMatBuffer(pMesh->GetBoneInverseBuffer());
        pBoneMatCS->SetOutputBuffer(m_BoneFinalMatBuffer);

        UINT iBoneCount = static_cast<UINT>(m_vecBones->size());
        pBoneMatCS->SetBoneCount(iBoneCount);
        pBoneMatCS->SetFrameIndex(m_FrameIdx);
        pBoneMatCS->SetNextFrameIdx(m_NextFrameIdx);
        pBoneMatCS->SetFrameRatio(m_Ratio);

        // ������Ʈ ���̴� ����
        pBoneMatCS->Execute();

        m_bFinalMatUpdate = true;
    }

    // t17 �������Ϳ� ������� ������(��������) ���ε�		
    m_BoneFinalMatBuffer->Binding(17);
}

void CAnimator3D::ClearData()
{
    m_BoneFinalMatBuffer->Clear(17);

    UINT iMtrlCount = MeshRender()->GetMaterialCount();
    Ptr<CMaterial> pMtrl = nullptr;
    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        pMtrl = MeshRender()->GetSharedMaterial(i);
        if (nullptr == pMtrl)
            continue;

        pMtrl->SetAnim3D(false); // Animation Mesh �˸���
        pMtrl->SetBoneCount(0);
    }
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
    UINT iBoneCount = _pMesh->GetBoneCount();
    if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
    {
        m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, false, nullptr);
    }
}

void CAnimator3D::SaveComponent(FILE* _File)
{
}

void CAnimator3D::LoadComponent(FILE* _File)
{
}

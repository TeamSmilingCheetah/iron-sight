#include "pch.h"
#include "Runtime/Public/Component/Animation/CAnimator3D.h"

#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Shader/CBoneMatrixCS.h"
#include "Runtime/Public/Component/Transform/CTransform.h"

CAnimator3D::CAnimator3D()
    : CComponent(COMPONENT_TYPE::ANIMATOR3D)
    , m_FrameCount(30)
    , m_CurTime(0.)
    , m_CurClip(0)
    , m_FrameIdx(0)
    , m_NextFrameIdx(0)
    , m_Ratio(0.f)
    , m_BoneFinalMatBuffer(nullptr)
	, m_BonePureMatBuffer(nullptr)
    , m_bFinalMatUpdate(false)
	, m_BindCaller(nullptr)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
	m_BonePureMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
    : CComponent(COMPONENT_TYPE::ANIMATOR3D)
    , m_vecClip(_origin.m_vecClip)
    , m_FrameCount(_origin.m_FrameCount)
    , m_CurTime(_origin.m_CurTime)
    , m_CurClip(_origin.m_CurClip)
    , m_FrameIdx(_origin.m_FrameIdx)
    , m_NextFrameIdx(_origin.m_NextFrameIdx)
    , m_Ratio(_origin.m_Ratio)
    , m_BoneFinalMatBuffer(nullptr)
	, m_BonePureMatBuffer(nullptr)
    , m_bFinalMatUpdate(false)
	, m_BindCaller(nullptr)
{
    m_BoneFinalMatBuffer = new CStructuredBuffer;
	m_BonePureMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
    if (nullptr != m_BoneFinalMatBuffer)
        delete m_BoneFinalMatBuffer;

	if (nullptr != m_BonePureMatBuffer)
		delete m_BonePureMatBuffer;
}

void CAnimator3D::FinalTick()
{
    m_CurTime = 0.f;

    // 현재 재생중인 Clip 의 시간을 진행한다.
    m_vecClipUpdateTime[m_CurClip] += EngineDT;

    if (m_vecClipUpdateTime[m_CurClip] >= m_vecClip[m_CurClip]->GetTimeLength())
    {
        m_vecClipUpdateTime[m_CurClip] = 0.f;

		m_CurClip = (m_CurClip + 1) % m_vecClip.size();
    }

    m_CurTime = m_vecClip[m_CurClip]->GetStartTime() + m_vecClipUpdateTime[m_CurClip];

    // 현재 프레임 인덱스 구하기
    double dFrameIdx = m_CurTime * static_cast<double>(m_FrameCount);
    m_FrameIdx = static_cast<int>(dFrameIdx);

    // 다음 프레임 인덱스
    if (m_FrameIdx >= m_vecClip[m_CurClip]->GetFrameLength() - 1)
        m_NextFrameIdx = m_FrameIdx; // 끝이면 현재 인덱스를 유지
    else
        m_NextFrameIdx = m_FrameIdx + 1;

    // 프레임간의 시간에 따른 비율을 구해준다.
    m_Ratio = static_cast<float>(dFrameIdx - (double)m_FrameIdx);

    // 컴퓨트 쉐이더 연산여부
    m_bFinalMatUpdate = false;
}


void CAnimator3D::Binding(CMeshRender* _Renderer)
{
    if (!m_bFinalMatUpdate)
    {
        // Animation3D Update Compute Shader
        static Ptr<CBoneMatrixCS> pBoneMatCS = new CBoneMatrixCS;

		Ptr<CAnimation> pCurAnim = m_vecClip[m_CurClip];

		const vector<tMTBone>* vecBones = pCurAnim->GetBones();
		m_vecFinalBoneMat.resize(vecBones->size());

		UINT iBoneCount = static_cast<UINT>(vecBones->size());

		if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
		{
			m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, false, nullptr);
			m_BonePureMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, false, nullptr);
		}

        pBoneMatCS->SetFrameDataBuffer(pCurAnim->GetBoneFrameDataBuffer());
        pBoneMatCS->SetOffsetMatBuffer(pCurAnim->GetBoneInverseBuffer());
        pBoneMatCS->SetOutputBuffer(m_BoneFinalMatBuffer);
		pBoneMatCS->SetPureOutputBuffer(m_BonePureMatBuffer);

        pBoneMatCS->SetBoneCount(iBoneCount);
        pBoneMatCS->SetFrameIndex(m_FrameIdx);
        pBoneMatCS->SetNextFrameIdx(m_NextFrameIdx);
        pBoneMatCS->SetFrameRatio(m_Ratio);

        // 업데이트 쉐이더 실행
        pBoneMatCS->Execute();

		// debug skeleton
		DrawDebugSkeleton(Vec4(0.f, 1.f, 0.f, 1.f), _Renderer->Transform()->GetWorldMat(), m_BonePureMatBuffer, m_vecClip[m_CurClip]->GetBoneParentBuffer(), false, 0.f);

        m_bFinalMatUpdate = true;
    }

	m_BindCaller = _Renderer;

    // t17 레지스터에 최종행렬 데이터(구조버퍼) 바인딩
    m_BoneFinalMatBuffer->Binding(17);
	m_BonePureMatBuffer->Binding(18);
}

void CAnimator3D::AddAnimClip(Ptr<CAnimation> _pAnim)
{
	m_vecClip.push_back(_pAnim);
	m_vecClipUpdateTime.push_back(0);
}

void CAnimator3D::SetAnimClip(const vector<Ptr<CAnimation>>& _vecAnim)
{
	m_vecClip = _vecAnim;
	m_vecClipUpdateTime.resize(_vecAnim.size());
}

void CAnimator3D::SetCurClip(int _Idx)
{
	assert(_Idx < m_FrameCount);

	m_vecClipUpdateTime[m_CurClip] = 0.f;
	m_CurTime = 0.;
	m_CurClip = _Idx;
}

void CAnimator3D::ClearData()
{
	if (!m_BindCaller) return;

    m_BoneFinalMatBuffer->Clear(17);
	m_BonePureMatBuffer->Clear(18);

    UINT iMtrlCount = m_BindCaller->GetMaterialCount();
    Ptr<CMaterial> pMtrl = nullptr;
    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        pMtrl = m_BindCaller->GetSharedMaterial(i);
        if (nullptr == pMtrl)
            continue;

        pMtrl->SetAnim3D(false); // Animation Mesh 알리기
        pMtrl->SetBoneCount(0);
    }

	m_BindCaller = nullptr;
}

void CAnimator3D::SaveComponent(FILE* _File)
{
}

void CAnimator3D::LoadComponent(FILE* _File)
{
}

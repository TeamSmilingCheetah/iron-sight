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
	, m_FPS(30)
	, m_FrameDuration(1.f / 30.f)
	, m_CurClipAccTime(0.f)
	, m_CurClipRatio(0.f)
	, m_NextClipAccTime(0.f)
	, m_NextClipRatio(0.f)
	, m_BlendAccTime(0.f)
	, m_BlendDuration(0.f)
	, m_BlendRatio(0.f)
	, m_CurClipIdx(0)
	, m_NextClipIdx(-1)
	, m_CurClipCurFrameIdx(0)
	, m_CurClipNextFrameIdx(0)
	, m_NextClipCurFrameIdx(0)
	, m_NextClipNextFrameIdx(0)
	, m_BoneFinalMatBuffer(nullptr)
	, m_BonePureMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_BindCaller(nullptr)
	, m_Active(true)
{
	m_BoneFinalMatBuffer = new CStructuredBuffer;
	m_BonePureMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_vecClip(_origin.m_vecClip)
	, m_FPS(_origin.m_FPS)
	, m_FrameDuration(_origin.m_FrameDuration)
	, m_CurClipAccTime(0.f)
	, m_CurClipRatio(0.f)
	, m_NextClipAccTime(0.f)
	, m_NextClipRatio(0.f)
	, m_BlendAccTime(0.f)
	, m_BlendDuration(0.f)
	, m_BlendRatio(0.f)
	, m_CurClipIdx(_origin.m_CurClipIdx)
	, m_NextClipIdx(-1)
	, m_CurClipCurFrameIdx(0)
	, m_CurClipNextFrameIdx(0)
	, m_NextClipCurFrameIdx(0)
	, m_NextClipNextFrameIdx(0)
	, m_BoneFinalMatBuffer(nullptr)
	, m_BonePureMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_BindCaller(nullptr)
	, m_vecBoneObject(_origin.m_vecBoneObject)
	, m_vecBoneWorldTransform(_origin.m_vecBoneWorldTransform)
	, m_Active(true)
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

	// BoneObject를 삭제함.
	DestroyObject(m_vecBoneObject[0]);
}

void CAnimator3D::Begin()
{
}

void CAnimator3D::FinalTick()
{
	if (!m_Active)
		return;

	bool curClipUpdated = false;

	// 다음 클립이 존재하는 경우 (현재 클립이 끝났으면 다음 클립으로 덮어씌우기 위해 먼저 계산함)
	if (m_NextClipIdx != -1)
	{
		// 다음 클립의 시간을 진행한다.
		m_NextClipAccTime += EngineDT;

		// 블렌드 시간을 진행한다.
		m_BlendAccTime += EngineDT;

		// 다음 클립의 현재 프레임 계산
		while (m_NextClipAccTime >= m_FrameDuration)
		{
			m_NextClipAccTime -= m_FrameDuration;
			++m_NextClipCurFrameIdx;
		}

		// 다음 클립이 종료되었다면
		// TEST : 일단 다음 클립이 현재 클립보다 먼저 끝나는 경우는 없다고 가정.
		assert(m_NextClipCurFrameIdx < m_vecClip[m_NextClipIdx]->GetFrameLength());

		// 다음 클립의 다음 프레임 계산
		if (m_NextClipCurFrameIdx + 1 == m_vecClip[m_NextClipIdx]->GetFrameLength())
		{
			m_NextClipNextFrameIdx = m_NextClipCurFrameIdx;
			m_NextClipRatio = 0.f;
		}
		else
		{
			m_NextClipNextFrameIdx = m_NextClipCurFrameIdx + 1;
			m_NextClipRatio = m_NextClipAccTime / m_FrameDuration;
		}


		// Blend 시간을 초과했다면
		if (m_BlendAccTime > m_BlendDuration)
		{
			// 다음 클립 정보를 현재 클립으로 옮김.
			m_CurClipIdx = m_NextClipIdx;
			m_CurClipAccTime = m_NextClipAccTime;
			m_CurClipRatio = m_NextClipRatio;
			m_CurClipCurFrameIdx = m_NextClipCurFrameIdx;
			m_CurClipNextFrameIdx = m_NextClipNextFrameIdx;

			// 초기화
			m_NextClipIdx = -1;
			m_NextClipCurFrameIdx = 0;
			m_NextClipNextFrameIdx = 0;
			m_BlendRatio = 0.f;
			m_BlendAccTime = 0.f;
			m_BlendDuration = 0.f;

			curClipUpdated = true;
		}
		else
		{
			// 현재 클립과 다음 클립의 보간 비율 계산
			m_BlendRatio = m_BlendAccTime / m_BlendDuration;
		}
	}

	if (!curClipUpdated)
	{
		// 현재 클립의 시간을 진행한다.
		m_CurClipAccTime += EngineDT;

		// 현재 클립의 현재 프레임 계산
		while (m_CurClipAccTime >= m_FrameDuration)
		{
			m_CurClipAccTime -= m_FrameDuration;
			++m_CurClipCurFrameIdx;
		}

		// 현재 클립이 끝난 경우
		if (m_CurClipCurFrameIdx >= GetFrameLength())
		{
			m_CurClipCurFrameIdx -= GetFrameLength();
		}

		// 현재 클립의 다음 프레임 계산
		if (m_CurClipCurFrameIdx + 1 == m_vecClip[m_CurClipIdx]->GetFrameLength())
		{
			m_CurClipNextFrameIdx = m_CurClipCurFrameIdx;
			m_CurClipRatio = 0.f;
		}
		else
		{
			m_CurClipNextFrameIdx = m_CurClipCurFrameIdx + 1;
			m_CurClipRatio = m_CurClipAccTime / m_FrameDuration;
		}

	}

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}


void CAnimator3D::Binding(CMeshRender* _Renderer)
{
	if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		static Ptr<CBoneMatrixCS> pBoneMatCS = new CBoneMatrixCS;

		Ptr<CAnimation> pCurAnim = m_vecClip[m_CurClipIdx];

		const vector<tMTBone>* vecBones = pCurAnim->GetBones();

		UINT iBoneCount = static_cast<UINT>(vecBones->size());

		// 같은 skeleton 쓴다는 가정 (당연하긴 함)
		if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
		{
			m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, false, nullptr);
			m_BonePureMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, true, nullptr);
			m_vecBoneWorldTransform.resize(iBoneCount);
		}

		pBoneMatCS->SetBoneCount(iBoneCount);
		pBoneMatCS->SetOffsetMatBuffer(pCurAnim->GetBoneInverseBuffer());
		pBoneMatCS->SetCurClipFrameBuffer(pCurAnim->GetBoneFrameDataBuffer());

		if (m_NextClipIdx != -1)
		{
			pBoneMatCS->SetNextClipFrameBuffer(m_vecClip[m_NextClipIdx]->GetBoneFrameDataBuffer());
		}

		pBoneMatCS->SetOutputBuffer(m_BoneFinalMatBuffer);
		pBoneMatCS->SetPureOutputBuffer(m_BonePureMatBuffer);

		pBoneMatCS->SetCurClipFrame(m_CurClipCurFrameIdx + m_CurClipRatio);
		pBoneMatCS->SetNextClipFrame(m_NextClipCurFrameIdx + m_NextClipRatio);
		pBoneMatCS->SetBlendRatio(m_BlendRatio);

		pBoneMatCS->Execute();

		// debug skeleton
		DrawDebugSkeleton(Vec4(0.f, 1.f, 0.f, 1.f), _Renderer->Transform()->GetWorldMat(), m_BonePureMatBuffer, m_vecClip[m_CurClipIdx]->GetBoneParentBuffer(), false, 0.f);

		// Bone Object World Transform 직접 세팅
		//  TODO : ZCompute shader로 world transform까지 계산해서 pipeline 넘겨주도록 개선하기
		m_BonePureMatBuffer->GetData(m_vecBoneWorldTransform.data());

		for (UINT i = 0; i < iBoneCount; ++i)
		{
			Matrix pureLocal = m_vecBoneWorldTransform[i].Transpose();

			if (!(pureLocal._11 == 0.f && pureLocal._22 == 0.f && pureLocal._33 == 0.f && pureLocal._41 == 0.f && pureLocal._42 == 0.f && pureLocal._43 == 0.f))
			{
				// 유효한 변환 정보가 있으므로 사용
				Matrix worldMat = pureLocal * _Renderer->Transform()->GetWorldMat();

				if (m_vecBoneObject[i])
					m_vecBoneObject[i]->Transform()->SetWorldMat(worldMat);
			}
			else
			{
				pureLocal = pCurAnim->GetBindLocal()[i];

				// Local좌표 등록
				Matrix worldMat = pureLocal * _Renderer->Transform()->GetWorldMat();

				if (m_vecBoneObject[i])
					m_vecBoneObject[i]->Transform()->SetWorldMat(worldMat);
			}
		}

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

	CreateBoneObject();
}

void CAnimator3D::SetAnimClip(const vector<Ptr<CAnimation>>& _vecAnim)
{
	m_vecClip = _vecAnim;

	CreateBoneObject();
}

void CAnimator3D::CreateBoneObject()
{
	// 이미 BoneObject가 있다면 생성하지 않는다.
	if (!m_vecBoneObject.empty())
	{
		// Deprecated : 기존에 Bone Object가 존재했다면 삭제하는 코드
		//DestroyObject(m_vecBoneObject[0]);
		//m_vecBoneObject.clear();

		return;
	}

	// Clip 0번의 Bone 정보를 가져옴
	const vector<tMTBone>* vecBones = m_vecClip[0]->GetBones();
	UINT BoneCount = static_cast<UINT>(vecBones->size());

	m_vecBoneObject.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; ++i)
	{
		m_vecBoneObject[i] = new CGameObject;
		m_vecBoneObject[i]->Transform()->SetManualUpdate(true);
	}

	// 부모 자식 관계 세팅. 1번부터 하는 이유는 0번은 자기자신을 부모라고 하고 있음
	// 모든 애니메이션이 같은 skeleton을 공유한다는 전제.

	for (UINT i = 1; i < BoneCount; ++i)
	{
		int parentIdx = vecBones->at(i).iParentIndx;

		m_vecBoneObject[i]->SetName(vecBones->at(i).strBoneName);
		m_vecBoneObject[parentIdx]->AddChild(m_vecBoneObject[i]);
	}

	m_vecBoneObject[0]->SetName(vecBones->at(0).strBoneName);

	// 자식 오브젝트로 넣음
	GetOwner()->AddChild(m_vecBoneObject[0]);
}

 void CAnimator3D::SetCurClip(int _ClipIdx)
{
	assert(_ClipIdx < m_vecClip.size());

	m_CurClipIdx = _ClipIdx;

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

 void CAnimator3D::SetCurClipFrame(int _FrameIdx)
 {
	 assert(_FrameIdx < m_vecClip[m_CurClipIdx]->GetFrameLength());

	 m_CurClipCurFrameIdx = _FrameIdx;
	 m_CurClipNextFrameIdx = min(_FrameIdx + 1, m_vecClip[m_CurClipIdx]->GetFrameLength() - 1);
	 m_CurClipAccTime = 0.f;
	 m_CurClipRatio = 0.f;

	 // 컴퓨트 쉐이더 연산여부
	 m_bFinalMatUpdate = false;
 }

 void CAnimator3D::SetCurClipBlend(int _ClipIdx, float _BlendTime)
 {
	 if (m_CurClipIdx == _ClipIdx)
		 return;

	 m_NextClipIdx = _ClipIdx;
	 m_NextClipCurFrameIdx = 0;
	 m_NextClipNextFrameIdx = 0;
	 m_NextClipAccTime = 0.f;
	 m_NextClipRatio = 0.f;

	 m_BlendDuration = _BlendTime;
	 m_BlendAccTime = 0.f;

	 // 컴퓨트 쉐이더 연산여부
	 m_bFinalMatUpdate = false;
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

void CAnimator3D::Crop(int _StartIdx, int _EndIdx)
{
	m_vecClip[m_CurClipIdx]->Crop(_StartIdx, _EndIdx);
}

void CAnimator3D::SaveComponent(FILE* _File)
{
	int ClipCnt = static_cast<int>(m_vecClip.size());
	fwrite(&ClipCnt, sizeof(int), 1, _File);

	for (int i = 0; i < ClipCnt; ++i)
	{
		SaveAssetRef(m_vecClip[i], _File);
	}

	// BoneObject 저장
	UINT BoneCount = static_cast<UINT>(m_vecBoneObject.size());
	fwrite(&BoneCount, sizeof(UINT), 1, _File);

	for (int i = 0; i < m_vecBoneObject.size(); ++i)
	{
		SaveObjectRef(m_vecBoneObject[i], _File);
	}
}

void CAnimator3D::LoadComponent(FILE* _File)
{
	int ClipCnt = 0;
	fread(&ClipCnt, sizeof(int), 1, _File);

	m_vecClip.resize(ClipCnt);
	for (int i = 0; i < ClipCnt; ++i)
	{
		LoadAssetRef(m_vecClip[i], _File);
	}

	// BoneObject 로드
	UINT BoneCount = 0;
	fread(&BoneCount, sizeof(UINT), 1, _File);

	m_vecBoneObject.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; ++i)
	{
		LoadObjectRef(m_vecBoneObject[i], _File);
	}
}


void CAnimator3D::LinkBoneObject()
{
	const vector<tMTBone>* vecBones = m_vecClip[0]->GetBones();

	UINT BoneCount = static_cast<UINT>(vecBones->size());

	m_vecBoneObject.resize(BoneCount);
	for (UINT i = 0; i < BoneCount; ++i)
	{
		m_vecBoneObject[i] = GetOwner()->GetChildByName(vecBones->at(i).strBoneName);
	}
}

void CAnimator3D::SetOwner(CGameObject* _Owner)
{
	CComponent::SetOwner(_Owner);

	// 복사로 생성된 경우, 복사된 Bone Object를 이름으로 연결한다.
	if (!m_vecClip.empty())
		LinkBoneObject();
}

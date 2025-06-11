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
	, m_AccTime(0.f)
	, m_FPS(30)
	, m_FrameDuration(1.f / m_FPS)
	, m_CurClip(0)
	, m_FrameIdx(0)
	, m_NextFrameIdx(0)
	, m_Ratio(0.f)
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
	, m_AccTime(0.f)
	, m_FPS(_origin.m_FPS)
	, m_FrameDuration(_origin.m_FrameDuration)
	, m_vecClip(_origin.m_vecClip)
	, m_CurClip(_origin.m_CurClip)
	, m_FrameIdx(_origin.m_FrameIdx)
	, m_NextFrameIdx(_origin.m_NextFrameIdx)
	, m_Ratio(_origin.m_Ratio)
	, m_BoneFinalMatBuffer(nullptr)
	, m_BonePureMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_BindCaller(nullptr)
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
	if (m_Active)
	{
		// 현재 재생중인 Clip 의 시간을 진행한다.
		m_AccTime += EngineDT;

		if (m_AccTime >= m_FrameDuration)
		{
			while (m_AccTime >= m_FrameDuration)
			{
				m_AccTime -= m_FrameDuration;
				++m_FrameIdx;
			}

			if (m_FrameIdx >= GetFrameLength())
			{
				m_FrameIdx -= GetFrameLength();
				m_CurClip = (m_CurClip + 1) % m_vecClip.size();
			}
		}
	}

	// 다음 프레임 인덱스
	if (m_FrameIdx >= m_vecClip[m_CurClip]->GetFrameLength() - 1)
		m_NextFrameIdx = m_FrameIdx; // 끝이면 현재 인덱스를 유지
	else
		m_NextFrameIdx = m_FrameIdx + 1;

	// 프레임간의 시간에 따른 비율을 구해준다.
	m_Ratio = m_AccTime / m_FrameDuration;

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
			m_BonePureMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, true, nullptr);
			m_vecBoneWorldTransform.resize(iBoneCount);
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

				//auto it = m_mapBoneObject.find(m_vecBoneObject[i]->GetName());
				//if (it != m_mapBoneObject.end())
				//	it->second->Transform()->SetWorldMat(worldMat);
			}
			else
			{
				pureLocal = pCurAnim->GetBindLocal()[i];

				// Local좌표 등록
				Matrix worldMat = pureLocal * _Renderer->Transform()->GetWorldMat();

				if (m_vecBoneObject[i])
					m_vecBoneObject[i]->Transform()->SetWorldMat(worldMat);

				//auto it = m_mapBoneObject.find(m_vecBoneObject[i]->GetName());
				//if (it != m_mapBoneObject.end())
				//	it->second->Transform()->SetWorldMat(worldMat);
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

 void CAnimator3D::SetCurClip(int _Idx)
{
	assert(_Idx < m_vecClip.size());

	m_CurClip = _Idx;
}

 void CAnimator3D::SetCurFrame(int _FrameIdx)
 {
	 assert(_FrameIdx < m_vecClip[m_CurClip]->GetFrameLength());

	 m_FrameIdx = _FrameIdx;
	 m_AccTime = 0.0;
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
	m_vecClip[m_CurClip]->Crop(_StartIdx, _EndIdx);
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
	for (int i = 0; i < ClipCnt; ++i)
	{
		Ptr<CAnimation> pAnimation;
		LoadAssetRef(pAnimation, _File);
		m_vecClip.push_back(pAnimation);
	}

	int ClipTimeCnt = 0;
	fread(&ClipTimeCnt, sizeof(int), 1, _File);

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

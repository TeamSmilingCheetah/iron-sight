#include "pch.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/System/Public/Rendering/Shader/CBoneMatrixCS.h"

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
	, m_CurClip(nullptr)
	, m_NextClip(nullptr)
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
	, m_mapClip(_origin.m_mapClip)
	, m_FPS(_origin.m_FPS)
	, m_FrameDuration(_origin.m_FrameDuration)
	, m_CurClipAccTime(0.f)
	, m_CurClipRatio(0.f)
	, m_NextClipAccTime(0.f)
	, m_NextClipRatio(0.f)
	, m_BlendAccTime(0.f)
	, m_BlendDuration(0.f)
	, m_BlendRatio(0.f)
	, m_CurClip(_origin.m_CurClip)
	, m_NextClip(nullptr)
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
	if (m_NextClip != nullptr)
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
		// TEST : 다음 클립이 현재 클립보다 먼저 끝나도 다음 클립을 loop 해준다
		if (m_NextClipCurFrameIdx >= m_NextClip->GetFrameLength())
		{
			m_NextClipCurFrameIdx -= m_NextClip->GetFrameLength();
		}

		// 다음 클립의 다음 프레임 계산
		if (m_NextClipCurFrameIdx + 1 == m_NextClip->GetFrameLength())
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
			m_CurClip = m_NextClip;
			m_CurClipAccTime = m_NextClipAccTime;
			m_CurClipRatio = m_NextClipRatio;
			m_CurClipCurFrameIdx = m_NextClipCurFrameIdx;
			m_CurClipNextFrameIdx = m_NextClipNextFrameIdx;

			// 초기화
			m_NextClip = nullptr;
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
		if (m_CurClipCurFrameIdx >= m_CurClip->GetFrameLength())
		{
			// 다음 애니메이션이 있거나 다음 애니메이션이 없지만 Repeat인 경우
			if (m_NextClip != nullptr || m_CurClip->IsLoop())
			{
				while (m_CurClipCurFrameIdx >= m_CurClip->GetFrameLength())
				{
					m_CurClipCurFrameIdx -= m_CurClip->GetFrameLength();
				}
			}

			// 다음 애니메이션이 없고 Repeat이 아닌 경우. 마지막 프레임에서 정지
			else
			{
				m_CurClipCurFrameIdx = m_CurClip->GetFrameLength() - 1;
				Pause();
			}
		}

		// 현재 클립의 다음 프레임 계산
		if (m_CurClipCurFrameIdx + 1 == m_CurClip->GetFrameLength())
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

		const vector<tMTBone>* vecBones = m_CurClip->GetBones();

		UINT iBoneCount = static_cast<UINT>(vecBones->size());

		// 같은 skeleton 쓴다는 가정 (당연하긴 함)
		if (m_BoneFinalMatBuffer->GetElementCount() != iBoneCount)
		{
			m_BoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, false, nullptr);
			m_BonePureMatBuffer->Create(sizeof(Matrix), iBoneCount, SRV_UAV, true, nullptr);
			m_vecBoneWorldTransform.resize(iBoneCount);
		}

		pBoneMatCS->SetBoneCount(iBoneCount);
		pBoneMatCS->SetOffsetMatBuffer(m_CurClip->GetBoneInverseBuffer());
		pBoneMatCS->SetCurClipFrameBuffer(m_CurClip->GetBoneFrameDataBuffer());

		if (m_NextClip != nullptr)
		{
			pBoneMatCS->SetNextClipFrameBuffer(m_NextClip->GetBoneFrameDataBuffer());
		}
		else
		{
			pBoneMatCS->SetNextClipFrameBuffer(nullptr);
		}

		pBoneMatCS->SetOutputBuffer(m_BoneFinalMatBuffer);
		pBoneMatCS->SetPureOutputBuffer(m_BonePureMatBuffer);

		pBoneMatCS->SetCurClipFrame(m_CurClipCurFrameIdx + m_CurClipRatio);
		pBoneMatCS->SetNextClipFrame(m_NextClipCurFrameIdx + m_NextClipRatio);
		pBoneMatCS->SetBlendRatio(m_BlendRatio);

		// TEST: minus blendratio
		if (m_BlendRatio < 0.f)
		{
			int a = 0;
		}

		pBoneMatCS->Execute();

		// debug skeleton
		DrawDebugSkeleton(Vec4(0.f, 1.f, 0.f, 1.f), _Renderer->Transform()->GetWorldMat(), m_BonePureMatBuffer, m_CurClip->GetBoneParentBuffer(), false, 0.f);

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
				pureLocal = m_CurClip->GetBindLocal()[i];

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
	// 이미 존재하는 클립은 바로 리턴
	if (m_mapClip.find(_pAnim->GetKey()) != m_mapClip.end())
		return;

	m_mapClip.emplace(_pAnim->GetKey(), _pAnim);

	if (m_CurClip == nullptr)
		m_CurClip = _pAnim;

	// 첫 등록이라면
	if (m_vecBoneObject.empty())
	{
		// 같은 계열의 모든 MeshRender에게 SkinRender을 알린다.
		CMeshRender* pMeshRender = MeshRender();

		// 1. 자신한테 직접 MeshRender가 있는 경우
		if (pMeshRender)
			pMeshRender->SetSkinRender(true);

		// 2. 자식에 있는 경우, 직속 자식인 MeshRender들에만 영향을 준다고 가정하고 설계됨.
		else
		{
			const vector<CGameObject*>& vecTestChild = GetOwner()->GetChild();
			for (auto child : vecTestChild)
			{
				pMeshRender = child->MeshRender();
				if (pMeshRender)
				{
					// 이미 SkinRender인 렌더러가 있다면, 이미 세팅된 경우
					if (pMeshRender->IsSkinRender())
						break;

					pMeshRender->SetSkinRender(true);
				}
			}
		}

		// Bone Object 생성
		CreateBoneObject();
	}
}

void CAnimator3D::AddAnimClips(const vector<Ptr<CAnimation>>& _vecAnim)
{
	for (auto clip : _vecAnim)
	{
		AddAnimClip(clip);
	}
}

void CAnimator3D::SetAnimClips(const vector<Ptr<CAnimation>>& _vecAnim)
{
	m_mapClip.clear();

	for (auto clip : _vecAnim)
	{
		m_mapClip.emplace(clip->GetKey(), clip);
	}

	// curClip을 받은 애니메이션 목록 중 하나로 지정해줌.
	m_CurClip = _vecAnim[0];

	if (m_vecBoneObject.empty())
		CreateBoneObject();
}

Ptr<CAnimation> CAnimator3D::GetAnimClip(const wstring _AnimName)
{
	auto iter = m_mapClip.find(_AnimName);

	if (iter == m_mapClip.end())
		return nullptr;

	return iter->second;
}

void CAnimator3D::CreateBoneObject()
{
	// Clip 0번의 Bone 정보를 가져옴
	const vector<tMTBone>* vecBones = m_mapClip.begin()->second->GetBones();
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

 void CAnimator3D::SetCurClip(const wstring& _AnimName)
{
	Ptr<CAnimation> pClip = GetAnimClip(_AnimName);

	if (pClip == nullptr)
		return;

	Play();

	m_CurClip = pClip;

	m_CurClipCurFrameIdx = 0;
	m_CurClipNextFrameIdx = 1;
	m_CurClipAccTime = 0.f;
	m_CurClipRatio = 0.f;

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

 void CAnimator3D::SetCurClipFrame(int _FrameIdx)
 {
	 if (_FrameIdx < 0 || _FrameIdx >= m_CurClip->GetFrameLength())
		 return;

	 m_CurClipCurFrameIdx = _FrameIdx;
	 m_CurClipNextFrameIdx = min(_FrameIdx + 1, m_CurClip->GetFrameLength() - 1);
	 m_CurClipAccTime = 0.f;
	 m_CurClipRatio = 0.f;

	 // 컴퓨트 쉐이더 연산여부
	 m_bFinalMatUpdate = false;
 }

 void CAnimator3D::SetCurClipBlend(const wstring& _AnimName, float _BlendTime)
 {
	 if (m_CurClip->GetKey() == _AnimName)
		 return;

	 Ptr<CAnimation> pNextClip = GetAnimClip(_AnimName);

	 // 지정한 clip이 없다면
	 if (pNextClip == nullptr)
		 return;

	 Play();

	 m_NextClip = pNextClip;
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
	m_CurClip->Crop(_StartIdx, _EndIdx);
}

void CAnimator3D::SaveComponent(FILE* _File)
{
	int ClipCnt = static_cast<int>(m_mapClip.size());
	fwrite(&ClipCnt, sizeof(int), 1, _File);

	for (auto iter = m_mapClip.begin(); iter != m_mapClip.end(); ++iter)
	{
		SaveAssetRef(iter->second, _File);
	}

	// CurClip, NextClip 저장
	SaveAssetRef(m_CurClip, _File);
	SaveAssetRef(m_NextClip, _File);

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

	Ptr<CAnimation> pClip;
	for (int i = 0; i < ClipCnt; ++i)
	{
		LoadAssetRef(pClip, _File);
		m_mapClip.emplace(pClip->GetKey(), pClip);
	}

	// CurClip, NextClip 로드
	LoadAssetRef(m_CurClip, _File);
	LoadAssetRef(m_NextClip, _File);

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
	const vector<tMTBone>* vecBones = m_mapClip[0]->GetBones();

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
	if (!m_mapClip.empty())
		LinkBoneObject();
}

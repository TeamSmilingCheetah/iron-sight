#include "pch.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

#include "Engine/Runtime/Public/Component/Physics/BoxCollider.h"
#include "Engine/Runtime/Public/Component/Physics/PlaneCollider.h"
#include "Engine/Runtime/Public/Component/Physics/MeshCollider.h"
#include "Engine/Runtime/Public/Component/Physics/RayCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CDecal.h"
#include "Engine/Runtime/Public/Component/Rendering/CParticleSystem.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CScriptMgr.h"

UINT CGameObject::GUID = 0;

const AABB NullAABB = {Vec3(FLT_MAX), Vec3(FLT_MIN)};

CGameObject::CGameObject()
	: MObjectID(GUID++)
	  , MComponentArray{}
	  , MRenderComponent(nullptr)
	  , MParent(nullptr)
	  , MLayerIdx(-1) // 레벨 및 레이어에 속하지 않음
	  , MNextLayerIdx(-1)
{
	// Status Boolean Setting
	SetActive(true);
	SetDead(false);
	SetDeactivated(false);
	SetLayerMove(false);

	// Trasform Is Default Component
	AddComponent(new CTransform);
}

CGameObject::CGameObject(const CGameObject& POrigin)
	: CEntity(POrigin)
	  , MObjectID(GUID++)
	  , MComponentArray{}
	  , MRenderComponent(nullptr)
	  , MParent(nullptr)
	  , MLayerIdx(-1)
{
	SetActive(POrigin.IsActive());
	SetDead(false);

	for (size_t i = 0; i < POrigin.MChildVector.size(); ++i)
	{
		AddChild(POrigin.MChildVector[i]->Clone());
	}

	for (auto* Component : POrigin.MComponentArray)
	{
		if (Component)
		{
			AddComponent(Component->Clone());
		}
	}

	for (size_t i = 0; i < POrigin.MScriptsVector.size(); ++i)
	{
		AddComponent(POrigin.MScriptsVector[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	DeleteArray(MComponentArray);
	DeleteVec(MScriptsVector);
	DeleteVec(MChildVector);
}

void CGameObject::Begin() const
{
	for (auto* Component : MComponentArray)
	{
		if (Component)
		{
			Component->Begin();
		}
	}

	for (size_t i = 0; i < MScriptsVector.size(); ++i)
	{
		MScriptsVector[i]->Begin();
	}

	for (size_t i = 0; i < MChildVector.size(); ++i)
	{
		MChildVector[i]->Begin();
	}
}

void CGameObject::Tick() const
{
	if (IsActive() && !IsDead())
	{
		for (auto* Component : MComponentArray)
		{
			if (Component)
			{
				Component->Tick();
			}
		}

		for (size_t i = 0; i < MScriptsVector.size(); ++i)
		{
			MScriptsVector[i]->Tick();
		}

		for (size_t i = 0; i < MChildVector.size(); ++i)
		{
			MChildVector[i]->Tick();
		}
	}
}

void CGameObject::FinalTick()
{
	if (IsActive())
	{
		// Layer 등록
		CLevelMgr::GetInst()->RegisterObject(this);

		if (!IsDead())
		{
			for (auto* Component : MComponentArray)
			{
				if (Component)
				{
					Component->FinalTick();
				}
			}
		}

		auto iter = MChildVector.begin();
		for (; iter != MChildVector.end();)
		{
			bool ChildDead = (*iter)->IsDead();

			(*iter)->FinalTick();

			if (ChildDead && !IsDead())
			{
				iter = MChildVector.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}

void CGameObject::Render() const
{
	if (IsActive())
	{
		MRenderComponent->Render();
	}
}

void CGameObject::AddComponent(FComponent* PComponent)
{
	COMPONENT_TYPE Type = PComponent->GetType();

	if (COMPONENT_TYPE::SCRIPT == Type)
	{
		// 오브젝트에 script 추가
		MScriptsVector.push_back(static_cast<CScript*>(PComponent));

		// script가 몇 번 위치에 존재하는지 기록
		MScriptShortcut[static_cast<CScript*>(PComponent)->GetScriptType()]
			= static_cast<UINT>(MScriptsVector.size()) - 1;

		// 부모 스크립트 정보로 들고 있는 경우, 해당 정보도 Shortcut에 등록해야 조회 시 문제가 없음
		if (static_cast<CScript*>(PComponent)->GetParentScriptType() != SCRIPT_TYPE::NONE)
		{
			MScriptShortcut[static_cast<CScript*>(PComponent)->GetParentScriptType()]
				= static_cast<UINT>(MScriptsVector.size()) - 1;
		}
	}
	else
	{
		// 입력으로 들어오는 컴포넌트와 이미 동일한 컴포넌트를 오브젝트가 가지고 있는 경우
		assert(!MComponentArray[static_cast<UINT>(Type)]);

		// 입력된 컴포넌트가 CRenderComponent의 자식 클래스 타입인지 확인
		if (dynamic_cast<FRenderComponent*>(PComponent))
		{
			assert(!MRenderComponent);
			MRenderComponent = static_cast<FRenderComponent*>(PComponent);
		}

		// 입력된 컴포넌트의 주소를 저장
		MComponentArray[static_cast<UINT>(Type)] = PComponent;
	}

	// 컴포넌트의 소유오브젝트를 세팅
	PComponent->SetOwner(this);

	// 컴포넌트 초기화
	PComponent->Init();
}

void CGameObject::AddChild(CGameObject* PChild)
{
	MChildVector.push_back(PChild);
	PChild->MParent = this;
	PChild->MLayerIdx = MLayerIdx;
}

void CGameObject::DeleteComponent(COMPONENT_TYPE PType)
{
	if (COMPONENT_TYPE::SCRIPT != PType)
	{
		// 입력으로 들어오는 컴포넌트가 없는 경우
		assert(MComponentArray[static_cast<UINT>(PType)]);

		// 입력된 컴포넌트가 CRenderComponent 의 자식클래스 타입인지 확인
		if (PType == COMPONENT_TYPE::TILEMAP || PType == COMPONENT_TYPE::MESHRENDER || PType ==
			COMPONENT_TYPE::PARTICLE_SYSTEM)
		{
			assert(MRenderComponent);
			MRenderComponent = nullptr;
		}

		// 해당 컴퍼넌트 삭제
		if (nullptr != MComponentArray[static_cast<UINT>(PType)])
		{
			delete MComponentArray[static_cast<UINT>(PType)];
		}

		MComponentArray[static_cast<UINT>(PType)] = nullptr;
	}
}

void CGameObject::DeleteScript(const wstring& PScriptName)
{
	CScript* pScript = CScriptMgr::GetInst()->GetScript(PScriptName);

	for (UINT i = 0; i < MScriptsVector.size(); ++i)
	{
		if (MScriptsVector[i]->GetName() == pScript->GetName())
		{
			// 해당 스크립트 관련 정보 일괄 제거
			MScriptsVector.erase(MScriptsVector.begin() + i);
			MScriptShortcut.erase(pScript->GetScriptType());

			// 부모 스크립트 정보로 들고 있는 경우, Trailing Delete
			if (pScript->GetParentScriptType() != SCRIPT_TYPE::NONE)
			{
				MScriptShortcut.erase(pScript->GetParentScriptType());
			}
		}
	}
}

UINT CGameObject::GetParentObjectID() const
{
	UINT ParentID;

	if (MParent != nullptr)
	{
		// 재귀 순회로 부모 ID 검색
		ParentID = MParent->GetParentObjectID();
	}
	else
	{
		ParentID = MObjectID;
	}

	return ParentID;
}

void CGameObject::SetObjectID(UINT PID)
{
	MObjectID = PID;
	GUID = max(GUID, PID + 1);
}

bool CGameObject::IsAncestor(const CGameObject* POther) const
{
	CGameObject* Parent = MParent;

	while (Parent)
	{
		if (Parent == POther)
		{
			return true;
		}

		Parent = Parent->MParent;
	}

	return false;
}

CGameObject* CGameObject::GetChildByName(const wstring& PName)
{
	queue<CGameObject*> ObjectQueue;
	ObjectQueue.emplace(this);

	while (!ObjectQueue.empty())
	{
		CGameObject* Object = ObjectQueue.front();
		ObjectQueue.pop();

		if (Object->GetName() == PName)
		{
			return Object;
		}

		for (CGameObject* pChild : Object->MChildVector)
		{
			ObjectQueue.emplace(pChild);
		}
	}

	return nullptr;
}

CAnimator3D* CGameObject::Animator3D() const
{
	CAnimator3D* AnimatorPtr = reinterpret_cast<CAnimator3D*>(GetComponent(COMPONENT_TYPE::ANIMATOR3D));

	if (!AnimatorPtr && MeshRender() && MeshRender()->IsSkinRender())
	{
		AnimatorPtr = MeshRender()->GetAnimator();
	}

	return AnimatorPtr;
}

void CGameObject::DisconnectWithLayer()
{
	// 소속 레이어가 없다면
	if (MLayerIdx == -1)
	{
		return;
	}

	CLayer* Layer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(MLayerIdx);
	Layer->DisconnectObject(this);
}

void CGameObject::DisconnecntWithParent()
{
	if (!MParent)
	{
		return;
	}

	auto iter = MParent->MChildVector.begin();

	for (; iter != MParent->MChildVector.end(); ++iter)
	{
		if (*iter == this)
		{
			MParent->MChildVector.erase(iter);
			MParent = nullptr;
			return;
		}
	}

	assert(nullptr);
}

void CGameObject::RegisterAsParent()
{
	// 소속 레이어가 없다면
	if (MLayerIdx == -1)
	{
		return;
	}

	CLayer* Layer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(MLayerIdx);
	Layer->RegisterAsParent(this);
}

/**
 * @brief 게임 오브젝트의 World에 배치된 Mesh 기준 바운딩 박스를 계산하는 함수
 * Collider를 배제한 순수 Object의 Mesh만 연산한다
 * @return 바운딩 박스의 최소, 최대 좌표 { Min, Max }
 */
AABB CGameObject::GetAABB() const
{
	// 바운딩 박스가 존재하기 위한 최소 조건 확인
	// 1. 메시 렌더러가 없는 경우 실패
	CMeshRender* MeshRenderPtr = MeshRender();
	CParticleSystem* ParticleSystemPtr = ParticleSystem();
	CDecal* DecalPtr = Decal();

	if (!MeshRenderPtr && !ParticleSystemPtr && !DecalPtr)
	{
		// LOG_ERROR_F("[GameObj] {}: Mesh Renderer가 존재하지 않음", WStringToString(GetName()));
		return NullAABB;
	}

	// 2. 메시가 없는 경우 실패
	Ptr<CMesh> MeshPtr = nullptr;
	if (MeshRenderPtr)
	{
		MeshPtr = MeshRenderPtr->GetMesh();
	}
	if (ParticleSystemPtr)
	{
		MeshPtr = ParticleSystemPtr->GetMesh();
	}
	if (DecalPtr)
	{
		MeshPtr = DecalPtr->GetMesh();
	}

	if (MeshPtr == nullptr)
	{
		LOG_ERROR("[GameObj] MeshPtr이 존재하지 않음");
		return NullAABB;
	}

	// TODO(KHJ): Animation 기반 Bounding Box Checking

	// Get Local Bound
	const auto& LocalBound = MeshPtr->GetLocalBound();
	Vec3 LocalMin = LocalBound[0];
	Vec3 LocalMax = LocalBound[1];

	// Set Min Thickness
	constexpr float MIN_THICKNESS = 1.0f;

	Vec3 size = LocalMax - LocalMin;

	if (size.x < MIN_THICKNESS)
	{
		float Center = (LocalMin.x + LocalMax.x) * 0.5f;
		LocalMin.x = Center - MIN_THICKNESS * 0.5f;
		LocalMax.x = Center + MIN_THICKNESS * 0.5f;
	}

	if (size.y < MIN_THICKNESS)
	{
		float Center = (LocalMin.y + LocalMax.y) * 0.5f;
		LocalMin.y = Center - MIN_THICKNESS * 0.5f;
		LocalMax.y = Center + MIN_THICKNESS * 0.5f;
	}

	if (size.z < MIN_THICKNESS)
	{
		float Center = (LocalMin.z + LocalMax.z) * 0.5f;
		LocalMin.z = Center - MIN_THICKNESS * 0.5f;
		LocalMax.z = Center + MIN_THICKNESS * 0.5f;
	}

	Vec3 Vertices[8] = {
		{LocalMin.x, LocalMin.y, LocalMin.z},
		{LocalMin.x, LocalMin.y, LocalMax.z},
		{LocalMin.x, LocalMax.y, LocalMin.z},
		{LocalMin.x, LocalMax.y, LocalMax.z},
		{LocalMax.x, LocalMin.y, LocalMin.z},
		{LocalMax.x, LocalMin.y, LocalMax.z},
		{LocalMax.x, LocalMax.y, LocalMin.z},
		{LocalMax.x, LocalMax.y, LocalMax.z}
	};

	const Matrix& WorldMatrix = Transform()->GetWorldMat();

	// Initialize World Bounding Box
	Vec3 WorldMin = XMVector3TransformCoord(Vertices[0], WorldMatrix);
	Vec3 WorldMax = WorldMin;

	// Calculate Min & Max
	for (int i = 1; i < 8; ++i)
	{
		Vec3 WorldPoint = XMVector3TransformCoord(Vertices[i], WorldMatrix);
		WorldMin.x = min(WorldMin.x, WorldPoint.x);
		WorldMin.y = min(WorldMin.y, WorldPoint.y);
		WorldMin.z = min(WorldMin.z, WorldPoint.z);

		WorldMax.x = max(WorldMax.x, WorldPoint.x);
		WorldMax.y = max(WorldMax.y, WorldPoint.y);
		WorldMax.z = max(WorldMax.z, WorldPoint.z);
	}

	// Return
	return {WorldMin, WorldMax};
}

/**
 * @brief 해당 오브젝트가 가진 Collider Component를 전부 반환하도록 하는 함수
 * @return Collider가 담긴 Vector
 */
vector<ColliderVariant> CGameObject::GetColliders() const
{
	vector<ColliderVariant> ColliderVector;

	if (PlaneCollider())
	{
		ColliderVector.push_back(PlaneCollider());
	}
	if (BoxCollider())
	{
		ColliderVector.push_back(BoxCollider());
	}
	if (SphereCollider())
	{
		ColliderVector.push_back(SphereCollider());
	}
	if (MeshCollider())
	{
		ColliderVector.push_back(MeshCollider());
	}
	if (RayCollider())
	{
		ColliderVector.push_back(RayCollider());
	}

	return ColliderVector;
}

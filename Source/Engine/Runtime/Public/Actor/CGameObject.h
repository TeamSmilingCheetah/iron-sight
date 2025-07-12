#pragma once
#include "Engine/Core/Public/CEntity.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"

/**
 * @brief 게임 월드를 구성하는 모든 오브젝트의 기본 단위
 * 오브젝트의 상태와 소속 레이어를 속성으로 보유하며, 컴포넌트 기반으로 동작, 계층 관계를 가질 수 있음
 *
 * @var GUID Global UID
 * @var MObjectID 오브젝트 ID
 * @var MComponentArray 컴포넌트 각각에 대응하는 Array
 * @var MRenderComponent 1개의 게임오브젝트 객체는 1개의 렌더 컴포넌트를 가질 수 있다.
 * @var MScriptsVector 보유 스크립트들
 * @var MScriptShortcut 스크립트 탐색 편의를 위한 shortcut
 * @var MParent 부모 오브젝트
 * @var MChildVector 자식 오브젝트들
 * @var MLayerIdx 오브젝트가 속해있는 레이어 인덱스 번호 (무소속은 -1)
 * @var MNextLayerIdx 오브젝트가 레이어 이동을 하면 이동할 레이어를 저장하는 변수
 * @var MObjectStatus 상태 통합 변수
 *      1 << 0 IsActive 오브젝트 활성화 여부
 *      1 << 1 IsDead 오브젝트의 상태가 삭제 예정 상태인지
 *      1 << 2 IsDeactivateNext 오브젝트가 비활성화 예정 상태인지
 *      1 << 3 IsLayerMoveNext 오브젝트가 레이어 이동 예정상태인지
 *      1 << 4 IsInFrustumPrevious 기존에 Frustum 내부에 있었는지
 */
class CGameObject :
	public CEntity
{
	friend class CLayer;
	friend class CTaskMgr;
	friend class CLevelMgr;

public:
	static UINT GUID;

private:
	UINT MObjectID;

	CComponent* MComponentArray[static_cast<UINT>(COMPONENT_TYPE::END)];
	CRenderComponent* MRenderComponent;
	vector<CScript*> MScriptsVector;
	unordered_map<SCRIPT_TYPE, UINT> MScriptShortcut;

	CGameObject* MParent;
	vector<CGameObject*> MChildVector;

	int MLayerIdx;
	int MNextLayerIdx;

	UINT8 MObjectStatus;

public:
	void Begin() const;
	void Tick() const;
	void FinalTick();
	void Render() const;

	void AddComponent(CComponent* PComponent);
	void AddChild(CGameObject* PChild);
	void DeleteComponent(COMPONENT_TYPE PType);
	void DeleteScript(const wstring& PScriptName);

	template <typename T>
	void AddComponentRecursive();

	CGameObject* GetChildByName(const wstring& PName);
	UINT GetParentObjectID() const;
	bool GetWorldBoundingBox(Vec3& PMin, Vec3& PMax) const;
	bool IsAncestor(const CGameObject* POther) const;

	// Getter & Setter
	UINT GetObjectID() const { return MObjectID; }

	const vector<CScript*>& GetScripts() const { return MScriptsVector; }
	const unordered_map<SCRIPT_TYPE, UINT>& GetScriptShortcut() const { return MScriptShortcut; }
	CComponent* GetComponent(COMPONENT_TYPE PType) const { return MComponentArray[static_cast<UINT>(PType)]; }
	CRenderComponent* GetRenderComponent() const { return MRenderComponent; }

	const vector<CGameObject*>& GetChild() const { return MChildVector; }
	CGameObject* GetParent() const { return MParent; }

	int GetLayerIdx() const { return MLayerIdx; }
	int GetNextLayerIdx() const { return MNextLayerIdx; }

	bool IsActive() const { return (MObjectStatus & (1 << 0)) != 0; }
	bool IsDead() const { return (MObjectStatus & (1 << 1)) != 0; }
	bool IsLayerMove() const { return (MObjectStatus & (1 << 3)) != 0; }
	bool IsInFrustumPrevious() const { return (MObjectStatus & (1 << 4)) != 0; }

	// 비활성화 되는 프레임에 대한 지연 처리는 IsDeactivated를 사용할 것
	// IsActive와 차이 : 아직 활성화 되어 있지만 다음 프레임에 비활성화 될건지 여부
	bool IsDeactivated() const { return (MObjectStatus & (1 << 2)) != 0; }

	void SetObjectID(UINT PID);
	void SetLayerIdx(int PIdx) { MLayerIdx = PIdx; }
	void SetNextLayerIdx(int PIdx) {
		SetLayerMove(true);
		MNextLayerIdx = PIdx;
	}
	void SetInFrustumPrevious(bool PStatus) {
		if (PStatus)
		{
			MObjectStatus |= (1 << 4);
		}
		else
		{
			MObjectStatus &= ~(1 << 4);
		}
	}

private:
	void DisconnectWithLayer();
	void DisconnecntWithParent();
	void RegisterAsParent();

	void LayerMoveDone() { SetLayerMove(false); }
	void SetActive(bool PStatus) {
		if (PStatus)
		{
			MObjectStatus |= (1 << 0);
		}
		else
		{
			MObjectStatus &= ~(1 << 0);
		}
	}
	void SetDead(bool PStatus) {
		if (PStatus)
		{
			MObjectStatus |= (1 << 1);
		}
		else
		{
			MObjectStatus &= ~(1 << 1);
		}
	}
	void SetDeactivated(bool PStatus) {
		if (PStatus)
		{
			MObjectStatus |= (1 << 2);
		}
		else
		{
			MObjectStatus &= ~(1 << 2);
		}
	}
	void SetLayerMove(bool PStatus) {
		if (PStatus)
		{
			MObjectStatus |= (1 << 3);
		}
		else
		{
			MObjectStatus &= ~(1 << 3);
		}
	}

public:
	// Get Component
	CTransform* Transform() const { return reinterpret_cast<CTransform*>(GetComponent(COMPONENT_TYPE::TRANSFORM)); }
	CMeshRender* MeshRender() const { return reinterpret_cast<CMeshRender*>(GetComponent(COMPONENT_TYPE::MESHRENDER)); }
	CCamera* Camera() const { return reinterpret_cast<CCamera*>(GetComponent(COMPONENT_TYPE::CAMERA)); }
	CCollider2D* Collider2D() const { return reinterpret_cast<CCollider2D*>(GetComponent(COMPONENT_TYPE::COLLIDER2D)); }
	CCollider3D* Collider3D() const { return reinterpret_cast<CCollider3D*>(GetComponent(COMPONENT_TYPE::COLLIDER3D)); }
	CColliderRay* ColliderRay() const{ return reinterpret_cast<CColliderRay*>(GetComponent(COMPONENT_TYPE::COLLIDERRAY)); }
	CFlipbookPlayer* FlipbookPlayer() const { return reinterpret_cast<CFlipbookPlayer*>(GetComponent(COMPONENT_TYPE::FLIPBOOKPLAYER)); }
	CTileMap* TileMap() const { return reinterpret_cast<CTileMap*>(GetComponent(COMPONENT_TYPE::TILEMAP)); }
	CLight2D* Light2D() const { return reinterpret_cast<CLight2D*>(GetComponent(COMPONENT_TYPE::LIGHT2D)); }
	CLight3D* Light3D() const { return reinterpret_cast<CLight3D*>(GetComponent(COMPONENT_TYPE::LIGHT3D)); }
	CParticleSystem* ParticleSystem() const { return reinterpret_cast<CParticleSystem*>(GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)); }
	CSkyBox* SkyBox() const { return reinterpret_cast<CSkyBox*>(GetComponent(COMPONENT_TYPE::SKYBOX)); }
	CDecal* Decal() const { return reinterpret_cast<CDecal*>(GetComponent(COMPONENT_TYPE::DECAL)); }
	CLandScape* LandScape() const { return reinterpret_cast<CLandScape*>(GetComponent(COMPONENT_TYPE::LANDSCAPE)); }
	CUI* UI() const { return reinterpret_cast<CUI*>(GetComponent(COMPONENT_TYPE::UI)); }
	CUIRender* UIRender() const { return reinterpret_cast<CUIRender*>(GetComponent(COMPONENT_TYPE::UIRENDER)); }
	CMeshCollider* MeshCollider() const { return reinterpret_cast<CMeshCollider*>(GetComponent(COMPONENT_TYPE::MESH_COLLIDER)); }
	CAnimator3D* Animator3D() const;

	// Special Member Function
	CLONE(CGameObject);
	CGameObject();
	CGameObject(const CGameObject& POrigin);
	~CGameObject() override;
};

/**
 * @brief 메인 오브젝트 이하 하위 오브젝트에 컴포넌트를 제공하는 함수
 * 타입 정보는 템플릿 인자로 전달하도록 처리
 */
template <typename T>
void CGameObject::AddComponentRecursive()
{
	for (auto ChildObject : MChildVector)
	{
		ChildObject->AddComponentRecursive<T>();
	}

	// FIXME(KHJ): 템플릿 특수화 시도할 경우 빌드 에러 발생, 매크로 관련으로 추정

	// Add Component
	if (this->MeshRender())
	{
		auto ChildMesh = this->MeshRender()->GetMesh();

		// 특이 케이스를 여기서 배제
		// TODO(KHJ): 너무 특정된 케이스, 배제 방식을 고려할 것
		if (ChildMesh.Get() && ChildMesh->GetKey() != L"Downtown_Alley_Scene_SkySphereMesh.mesh")
		{
			this->AddComponent(new T);
		}
	}
}

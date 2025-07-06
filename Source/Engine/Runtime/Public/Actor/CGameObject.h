#pragma once
#include "Engine/Core/Public/CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;
class CMeshCollider;

class CGameObject :
	public CEntity
{
	friend class CLayer;
	friend class CTaskMgr;
	friend class CLevelMgr;

public:
	static UINT GUID;

private:
	UINT m_ObjectID;

	CComponent* m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
	CRenderComponent* m_RenderCom; // 1개의 게임오브젝트 객체는 1개의 렌더컴포넌트를 가질 수 있다.
	vector<CScript*> m_vecScripts; // 보유 스크립트들
	unordered_map<SCRIPT_TYPE, UINT> m_scriptShortcut; // 스크립트 탐색 편의를 위한 shortcut

	CGameObject* m_Parent; // 부모 오브젝트
	vector<CGameObject*> m_vecChild; // 자식 오브젝트들

	int m_LayerIdx; // 오브젝트가 속해있는 레이어 인덱스 번호, -1 : 무소속
	int m_NextLayerIdx;	// 오브젝트가 레이어 이동을 하면 이동할 레이어를 저장하는 변수

	bool m_Active;		// 오브젝트 활성화 여부

	bool m_Dead;		// 오브젝트의 상태가 삭제 예정 상태인지
	bool m_Deactivate;	// 오브젝트가 비활성화 예정 상태인지
	bool m_LayerMove;	// 오브젝트가 레이어 이동 예정상태인지

public:
	void Begin();
	void Tick();
	void FinalTick();
	void Render();

	void AddComponent(CComponent* _Component);
	void AddChild(CGameObject* _Child);
	void DeleteComponent(COMPONENT_TYPE _Type);
	void DeleteScript(wstring& _ScriptName);

	UINT GetObjectID() const { return m_ObjectID; }
	UINT GetParentObjectID() const;
	void SetObjectID(UINT _ID);

	const unordered_map<SCRIPT_TYPE, UINT>& GetScriptShortcut() const { return m_scriptShortcut; }
	CGameObject* GetParent() const { return m_Parent; }
	CComponent* GetComponent(COMPONENT_TYPE _Type) const { return m_arrCom[static_cast<UINT>(_Type)]; }
	CRenderComponent* GetRenderComponent() const { return m_RenderCom; }

	int GetLayerIdx() const { return m_LayerIdx; }
	int GetNextLayerIdx() const { return m_NextLayerIdx; }

	// 비활성화 되는 프레임에 대한 지연 처리는 IsDeactivated를 사용할 것
	bool IsActive() const { return m_Active; }
	bool IsDead() const { return m_Dead; }
	bool IsAncestor(CGameObject* _Other);
	bool IsLayerMove() const { return m_LayerMove; }
	// IsActive와 차이 : 아직 활성화 되어있지만 다음 프레임에 비활성화 될건지 여부
	bool IsDeactivated() const { return m_Deactivate; }

private:
	void SetActive(bool _b) { m_Active = _b; }

public:
	void SetLayerIdx(int _Idx) { m_LayerIdx = _Idx; }
	void SetNextLayerIdx(int _Idx) {
		m_LayerMove = true;
		m_NextLayerIdx = _Idx;
	}

public:
	const vector<CGameObject*>& GetChild() const { return m_vecChild; }
	const vector<CScript*>& GetScripts() const { return m_vecScripts; }

	CGameObject* GetChildByName(const wstring& _Name);

	class CTransform* Transform() const { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
	class CMeshRender* MeshRender() const { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
	class CCamera* Camera() const { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
	class CCollider2D* Collider2D() const { return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D); }
	class CCollider3D* Collider3D() const { return (CCollider3D*)GetComponent(COMPONENT_TYPE::COLLIDER3D); }
	class CColliderRay* ColliderRay() const { return (CColliderRay*)GetComponent(COMPONENT_TYPE::COLLIDERRAY); }
	class CFlipbookPlayer* FlipbookPlayer() const { return (CFlipbookPlayer*)GetComponent(COMPONENT_TYPE::FLIPBOOKPLAYER); }
	class CTileMap* TileMap() const { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
	class CLight2D* Light2D() const { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
	class CLight3D* Light3D() const { return (CLight3D*)GetComponent(COMPONENT_TYPE::LIGHT3D); }
	class CAnimator3D* Animator3D();
	class CParticleSystem* ParticleSystem() const { return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM); }
	class CSkyBox* SkyBox() const { return (CSkyBox*)GetComponent(COMPONENT_TYPE::SKYBOX); }
	class CDecal* Decal() const { return (CDecal*)GetComponent(COMPONENT_TYPE::DECAL); }
	class CLandScape* LandScape() const { return (CLandScape*)GetComponent(COMPONENT_TYPE::LANDSCAPE); }
	class CUI* UI() const { return (CUI*)GetComponent(COMPONENT_TYPE::UI); }
	class CUIRender* UIRender() const { return (CUIRender*)GetComponent(COMPONENT_TYPE::UIRENDER); }
	class CMeshCollider* MeshCollider() const { return (CMeshCollider*)GetComponent(COMPONENT_TYPE::MESH_COLLIDER); }

	bool GetWorldBoundingBox(Vec3& PMin, Vec3& PMax) const;

private:
	void DisconnectWithLayer();
	void DisconnecntWithParent();
	void RegisterAsParent();

	void LayerMoveDone() { m_LayerMove = false; }

public:
	CLONE(CGameObject);
	CGameObject();
	CGameObject(const CGameObject& _Origin);
	~CGameObject() override;
};

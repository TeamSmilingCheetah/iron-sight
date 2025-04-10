#pragma once
#include "Engine/Core/Public/CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
	public CEntity
{
	friend class CLayer;
	friend class CTaskMgr;

public:
	static UINT GUID;

private:
	UINT m_ObjectID;

	CComponent* m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
	CRenderComponent* m_RenderCom; // 1개의 게임오브젝트 객체는 1개의 렌더컴포넌트를 가질 수 있다.
	vector<CScript*> m_vecScripts; // 보유 스크립트들

	CGameObject* m_Parent; // 부모 오브젝트
	vector<CGameObject*> m_vecChild; // 자식 오브젝트들

	int m_LayerIdx; // 오브젝트가 속해있는 레이어 인덱스 번호, -1 : 무소속

	bool m_Active; // 오브젝트 활성화 여부
	bool m_Dead; // 오브젝트의 상태가 삭제 예정 상태인지

public:
	void Begin();
	void Tick();
	void FinalTick();
	void Render();

	void AddComponent(CComponent* _Component);
	void AddChild(CGameObject* _Child);
	void DeleteComponent(COMPONENT_TYPE _Type);
	void DeleteScript(wstring& _SciprtName);

	UINT GetObjectID() const { return m_ObjectID; }
	void SetObjectID(UINT _ID);

	CGameObject* GetParent() const { return m_Parent; }
	CComponent* GetComponent(COMPONENT_TYPE _Type) const { return m_arrCom[static_cast<UINT>(_Type)]; }
	CRenderComponent* GetRenderComponent() const { return m_RenderCom; }

	void SetActive(bool _b) { m_Active = _b; }

	int GetLayerIdx() const { return m_LayerIdx; }
	bool IsActive() const { return m_Active; }
	bool IsDead() const { return m_Dead; }
	bool IsAncestor(CGameObject* _Other);

	void SetLayerIdx(int _Idx) { m_LayerIdx = _Idx; }

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

private:
	void DisconnectWithLayer();
	void DisconnecntWithParent();
	void RegisterAsParent();
	
public:
	CLONE(CGameObject);
	CGameObject();
	CGameObject(const CGameObject& _Origin);
	~CGameObject() override;
};

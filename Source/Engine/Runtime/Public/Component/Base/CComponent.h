#pragma once

class FMeshCollider;
class CUIRender;
class CUI;
class FLandscape;
class CDecal;
class CSkyBox;
class CParticleSystem;
class CAnimator3D;
class CLight3D;
class CLight2D;
class CFlipbookPlayer;
class FCollider3D;
class CTileMap;
class FColliderRay;
class FCollider2D;
class CCamera;
class CMeshRender;
class CTransform;
class CStateMachine;
class CGameObject;

class CComponent :
	public CEntity
{
private:
	CGameObject* m_Owner;
	const COMPONENT_TYPE m_Type;

public:
	COMPONENT_TYPE GetType() const { return m_Type; }
	CGameObject* GetOwner() const { return m_Owner; }

	virtual void Init()
	{
	} // 생성자 시점 이후에, GameObject 에 AddComponent 된 직후 호출되는 함수

	virtual void Begin()
	{
	}

	virtual void Tick()
	{
	}
	virtual void FinalTick() = 0;

	virtual void SetOwner(CGameObject* _Owner) { m_Owner = _Owner; }

	CComponent* Clone() override = 0;

	void SaveToLevel(FILE* _File) override;
	void LoadFromLevel(FILE* _File) override;

	virtual void SaveComponent(FILE* _File) = 0;
	virtual void LoadComponent(FILE* _File) = 0;
	virtual void LoadComponentReference() {}

	CTransform* Transform() const;
	CMeshRender* MeshRender() const;
	CCamera* Camera() const;
	FCollider2D* Collider2D() const;
	FCollider3D* Collider3D() const;
	FColliderRay* ColliderRay() const;
	CFlipbookPlayer* FlipbookPlayer() const;
	CTileMap* TileMap() const;
	CLight2D* Light2D() const;
	CLight3D* Light3D() const;
	CAnimator3D* Animator3D() const;
	CParticleSystem* ParticleSystem() const;
	CSkyBox* SkyBox() const;
	CDecal* Decal() const;
	FLandscape* LandScape() const;
	CUI* UI() const;
	CUIRender* UIRender() const;
	FMeshCollider* MeshCollider() const;
	CStateMachine* StateMachine() const;

	CComponent(COMPONENT_TYPE _TYPE);
	~CComponent() override;
};

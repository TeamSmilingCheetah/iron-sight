#pragma once
#include "Engine/Core/Public/CEntity.h"

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
class FBoxCollider;
class CTileMap;
class FRayCollider;
class FPlaneCollider;
class CCamera;
class CMeshRender;
class CTransform;
class CStateMachine;
class CGameObject;

class FComponent :
	public CEntity
{
private:
	const COMPONENT_TYPE Type;
	CGameObject* Owner;

public:
	virtual void Init()
	{
		// 생성자 시점 이후에, GameObject 에 AddComponent 된 직후 호출되는 함수
	}

	virtual void Begin()
	{
	}

	virtual void Tick()
	{
	}

	virtual void FinalTick() = 0;

	CTransform* Transform() const;
	CMeshRender* MeshRender() const;
	CCamera* Camera() const;
	FPlaneCollider* PlaneCollider() const;
	FBoxCollider* BoxCollider() const;
	FRayCollider* RayCollider() const;
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

	// Getter & Setter
	COMPONENT_TYPE GetType() const { return Type; }
	CGameObject* GetOwner() const { return Owner; }

	virtual void SetOwner(CGameObject* InOwner) { Owner = InOwner; }

	// Save & Load
	void SaveToLevel(FILE* InFile) override;
	void LoadFromLevel(FILE* InFile) override;

	virtual void SaveComponent(FILE* InFile) = 0;
	virtual void LoadComponent(FILE* InFile) = 0;

	virtual void LoadComponentReference()
	{
	}

	// Special Member Function
	FComponent(COMPONENT_TYPE InType);
	~FComponent() override;
	FComponent* Clone() override = 0;
};

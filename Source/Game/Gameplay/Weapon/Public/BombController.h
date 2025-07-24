#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class BombController :
	public CScript
{
private:
	Ptr<CSound> m_TinnitusSound;

	int			m_TinnitusSoundIdx;

	CGameObject* m_WeaponOwner;

	float m_DMG;
	float m_AccTime;

	float m_MaxLength;

public:
	void SetWeaponOwner(CGameObject* _Object) { m_WeaponOwner = _Object; }

public:
	void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;
	void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
		CCollider3D* _OtherCollider) override;


	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(BombController);
	BombController();
	~BombController() override;
};


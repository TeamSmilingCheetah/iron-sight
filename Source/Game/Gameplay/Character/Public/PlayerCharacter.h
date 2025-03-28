#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

class CGameObject;

class PlayerCharacter :
	public CScript
{
private:
	Vec3 m_velocity;

	float m_Acceleration;
	float m_Deceleration;
	float m_MaxSpeed;
	float m_PlayerSpeed;
	float m_MouseSensitivity;
	float m_PaperBurnIntence;

	bool m_bShoot;

	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

	vector<CGameObject*> m_vecWeaponSlot;
	CGameObject* m_CurWeapon;

public:
	void Begin() override;
	void Tick() override;
	void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	                  CCollider2D* _OtherCollider) override;
	void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	             CCollider2D* _OtherCollider) override;
	void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	                CCollider2D* _OtherCollider) override;

private:
	void UpdatePosition();
	void UpdateRotation();
	void PlayerAttack();

public:
	void SetCurWeapon(CGameObject* _Weapon) { m_CurWeapon = _Weapon; }
	CGameObject* GetCurWeapon() { return m_CurWeapon; }
 
	float GetCurMouseSensitivity() { return m_MouseSensitivity; }
	bool IsShot() { return m_bShoot; }

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	CLONE(PlayerCharacter);
	PlayerCharacter();
	~PlayerCharacter() override;
};

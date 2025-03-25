#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

class CGameObject;
class CLandScape;

class PlayerCharacter :
	public CScript
{
private:
	// 질량 시스템
	Vec3 m_Force;				// 누적 힘
	Vec3 m_Velocity;			// 속도
	Vec3 m_Accel;				// 가속도
	Vec3 m_GravidyVelocity;		// 중력 속도
	float m_Mass;				// 질량
	float m_Friction;			// 마찰계수
	float m_MaxSpeed;			// 최대 속력 제한
	float m_GravityAccel;		// 중력가속도 크기
	float m_GravityMaxSpeed;	// 중력으로 인해서 발생하는 속도의 최대 제한치
	float m_JumpPower;			// 점프용

	bool m_IsGround;				// 지상위에 서있는지 판정

	float m_MouseSensitivity;
	float m_PaperBurnIntence;

	bool m_bShoot;

	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

	CGameObject* m_CurWeapon;
	vector<CGameObject*> m_vecWeaponSlot;	// 무기 슬롯
	vector<Vec3> m_vecCollisionNormal; // 충돌 노말 벡터

public:
	void Begin() override;
	void Tick() override;
	void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
	                  CCollider3D* _OtherCollider) override;
	void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,
	             CCollider3D* _OtherCollider) override;
	void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,
	                CCollider3D* _OtherCollider) override;

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;

private:
public:
	void UpdatePosition();
	void UpdateRotation();
	void PlayerAttack();

	void MoveCalcul();
	void gravityCalcul();
	void ColliderCalcul();

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

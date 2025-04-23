#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class CGameObject;
class CLandScape;

#define PRIMARY_FIRST 0
#define PRIMARY_SECOND 1
#define SECONDARY_FIRST 2
#define THROWABLE_FIRST 3
#define THROWABLE_SECOND 4
#define NONE_WEAPON 5

struct tSlot
{
	ITEM_TYPE		Type;
	CGameObject*	Object;
};

class PlayerCharacter :
	public CScript
{
private:
	CGameObject* m_MainCamera;

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
	bool m_bCanThrow;
	bool m_bCanEquip;
	bool m_bThrowBoom;

	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

	CGameObject* m_CurWeapon;
	CGameObject* m_CollObject;
	int			 m_CurWeaponIdx;


	vector<tSlot> m_vecWeaponSlot;	// 무기 슬롯
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


	virtual void BeginOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider) override;
	virtual void Overlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider) override;
	virtual void EndOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider) override;


	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;

private:
	void UpdatePosition();
	void UpdateRotation();

	void PlayerReload();
	void PlayerAttack();
	void PlayerInteractWeapon();

public:
	void MoveCalcul();
	void gravityCalcul();
	void ColliderCalcul();
	

public:
	CGameObject* GetRayTarget() const { return m_CollObject; }

	void SetCurWeapon(CGameObject* _Weapon) { m_CurWeapon = _Weapon; }
	void SetShot(bool _Shot) { m_bShoot = _Shot; }
	void SetThrow(bool _Throw) { m_bCanThrow = _Throw; }
	void SetThrowBoom(bool _Boom) { m_bThrowBoom = _Boom; }

	CGameObject* GetCurWeapon() { return m_CurWeapon; }
	CGameObject* GetPlayeChildMeshObject(wstring& _str);

	int GetCurWeaponIdx() { return m_CurWeaponIdx; }
	float GetCurMouseSensitivity() { return m_MouseSensitivity; }
	bool IsShot() { return m_bShoot; }
	bool IsThrow() { return m_bCanThrow; }

	void EquipSlot(CGameObject* _Item);
	void ReleaseSlot(ITEM_TYPE _Type, int _Count);

	// TODO: 데이터 구조 개선
	void AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot);
	void DetachItem(CGameObject* _Item);

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(PlayerCharacter);
	PlayerCharacter();
	~PlayerCharacter() override;
};

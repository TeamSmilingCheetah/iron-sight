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

enum class HEAL_STATE
{
	HEAL,
	BOOST,
	NONE,
};

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

	CGameObject* m_HandMeshObj;
	CGameObject* m_BackMeshObj;

	// 질량 시스템
	Vec3 m_Force;				// 누적 힘
	Vec3 m_Velocity;			// 속도
	Vec3 m_Accel;				// 가속도
	Vec3 m_GravityVelocity;		// 중력 속도
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

	// ======
	// Status
	// ======
	const float		m_MaxHP;	// 최대 체력
	const float		m_SemiMaxHP;	// 붕대, 구급상자로 회복할 수 있는 최대치의 비율
	float			m_CurHP;	// 현재 체력

	const float		m_MaxBoost;	// 최대 Boost
	float			m_CurBoost;	// 에너지

	HEAL_STATE		m_HealState;

	float			m_RemainTime;
	float			m_TotalTime;
	float			m_HealAmount;	// heal 또는 boost 양

	float			m_BoostRemainTime;
	const float		m_BoostTotalTime;	// boost가 수행되는 시간 단위
	const float		m_BoostUnit;	// 시간 지나면 boost가 빠질 단위
	const float		m_BoostHP;		// boost를 사용해서 회복할 체력 양


	// =======
	// UI 관리
	// =======

	// 인벤토리 UI
	CGameObject*	m_InventoryCanvasUI;
	bool			m_InventoryOpened;

	// 방위 UI
	CGameObject*	m_CardinalImageUI;

	// HP UI
	CGameObject*	m_HPUI;

	// 아이템 사용 UI
	CGameObject*	m_ItemUseUI;

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
	void PlayerMove();
	void PlayerView();

	void PlayerReload();
	void PlayerAttack();
	void PlayerInteractWeapon();

	void PlayerControlUI();
	void PlayerHeal();

	void MoveCalcul();
	void gravityCalcul();
	void ColliderCalcul();

	// TODO: 데이터 구조 개선
	void AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot);
	void DetachItem(CGameObject* _Item);
	

public:
	CGameObject* GetRayTarget() const { return m_CollObject; }

	void SetCurWeapon(CGameObject* _Weapon) { m_CurWeapon = _Weapon; }
	void SetShot(bool _Shot) { m_bShoot = _Shot; }
	void SetThrow(bool _Throw) { m_bCanThrow = _Throw; }
	void SetThrowBoom(bool _Boom) { m_bThrowBoom = _Boom; }

	CGameObject* GetCurWeapon() { return m_CurWeapon; }
	//CGameObject* GetPlayeChildMeshObject(const wstring& _str);

	int GetCurWeaponIdx() const { return m_CurWeaponIdx; }
	float GetCurMouseSensitivity() const { return m_MouseSensitivity; }
	bool IsShot() const { return m_bShoot; }
	bool IsThrow() const { return m_bCanThrow; }

	void EquipSlot(CGameObject* _Item);
	void ReleaseSlot(ITEM_TYPE _Type, int _Count);

	void TriggerHeal(ITEM_TYPE _HealType);

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(PlayerCharacter);
	PlayerCharacter();
	~PlayerCharacter() override;
};

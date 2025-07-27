#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

class CGameObject;
class CLandScape;
class KillinfoUIScript;
class InventoryController;
class CameraController;
class CameraEffect;

enum class MOTION_STATE : uint8_t
{
	STAND,	// 서 있음
	CROUCH,	// 앉아 있음
	PRONE,	// 엎드려 있음
};

enum class ACTION_STATE : uint8_t
{
	JUMP,

	GUN_FIRE,
	GUN_RELOAD,

	GRENADE_PREPARE,
	GRENADE_THROW_LOW,
	GRENADE_THROW_HIGH,

	BANDAGE,
	MED_KIT,
	FIRST_AID_KIT,

	ENERGY_DRINK,
	PAIN_KILLER,
	ADRENALINE_SYRINGE,

	DEAD,

	NONE,
};

class PlayerCharacter :
	public CScript
{
private:
	Ptr<CSound>		m_HitSound;
	Ptr<CSound>		m_FootstepSound;
	Ptr<CSound>		m_RunFootstepSound;

	int				m_HitSoundIdx;
	int				m_FootstepSoundIdx;
	int				m_RunFootstepSoundIdx;


	CGameObject*	m_MainCamera;

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
	bool m_bLean;
	bool m_bMouseActive;			//  마우스 활성화

	float m_MouseSensitivity;

	bool m_bShoot;
	bool m_bReloading;
	bool m_bReloadingEnd;
	bool m_bCanThrow;
	bool m_bThrowBoom;
	bool m_bHitSoundPlayed;
	bool m_bFirstFootStep;

	float m_HitSoundAccTime;
	float m_FootStepSoundAccTime;

	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

	CGameObject* m_CollObject;
	CGameObject* m_HeadColl;

	vector<Vec3> m_vecCollisionNormal; // 충돌 노말 벡터

	// ======
	// Script
	// ======
	CameraController*		m_CamScript;
	InventoryController*	m_InventoryScript;
	KillinfoUIScript*		m_KillinfoScript;
	CameraEffect*			m_CameraEffect;

	// ======
	// Status
	// ======
	const float		m_MaxHP;		// 최대 체력
	const float		m_SemiMaxHP;	// 붕대, 구급상자로 회복할 수 있는 최대치의 비율
	float			m_CurHP;		// 현재 체력

	const float		m_MaxBoost;		// 최대 Boost
	float			m_CurBoost;		// 에너지

	ITEM_TYPE		m_HealType;		// 힐 아이템 종류

	float			m_HealRemainTime;
	float			m_HealTotalTime;
	float			m_HealAmount;		// heal 또는 boost 양

	float			m_BoostRemainTime;
	const float		m_BoostTotalTime;	// boost가 수행되는 시간 단위
	const float		m_BoostUnit;		// 시간 지나면 boost가 빠질 단위
	float			m_BoostSpeed;		// 부스트로 인한 이동속도 보정

	int				m_KillCounts;	// 킬 카운트

	// =====
	// State
	// =====
	MOTION_STATE		m_MotionState;
	ACTION_STATE		m_ActionState;

	// =======
	// UI 관리
	// =======
	CGameObject*	m_InventoryCanvasUI;	// 인벤토리 UI
	bool			m_InventoryOpened;

	CGameObject*	m_CardinalImageUI;		// 방위 UI
	CGameObject*	m_HPUI;					// HP UI
	CGameObject*	m_ItemUseUI;			// 아이템 사용 UI
	CGameObject*	m_ReloadUI;				// 재장전 UI

public:
	void Begin() override;
	void Tick() override;

	void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) override;
	void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) override;
	void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) override;

	void BeginOverlap(FColliderRay* PRayCollider, CGameObject* POtherObject, FCollider3D* P3DCollider) override;
	void Overlap(FColliderRay* PRayCollider, CGameObject* POtherObject, FCollider3D* P3DCollider) override;
	void EndOverlap(FColliderRay* PRayCollider, CGameObject* POtherObject, FCollider3D* P3DCollider) override;

	void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) override;
	void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) override;
	void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) override;

	void BeginOverlap(FCollider3D* P3DCollider, CGameObject* POtherObject, FMeshCollider* PMeshCollider) override;
	void Overlap(FCollider3D* P3DCollider, CGameObject* POtherObject, FMeshCollider* PMeshCollider) override;
	void EndOverlap(FCollider3D* P3DCollider, CGameObject* POtherObject, FMeshCollider* PMeshCollider) override;

private:
	void LoadPlayerSounds();

	void PlayerMove();
	void PlayerView();
	void PlayerStance();
	
	void PlayerControlWeapon();

	void PlayerControlUI();
	void PlayerHeal();

	void InitMove();
	void UpdateMove();
	void UpdateGravity();
	void UpdateCollision();
	void AnimationControl();
	
	

public:
	CGameObject* GetRayTarget() const { return m_CollObject; }

	void SetShot(bool PShot) { m_bShoot = PShot; }
	void SetThrow(bool PThrow) { m_bCanThrow = PThrow; }
	void SetThrowBoom(bool PBoom) { m_bThrowBoom = PBoom; }
	void SetReloading(bool PReloading) { m_bReloading = PReloading; }
	void SetReloadingEnd(bool PReloadingEnd) { m_bReloadingEnd = PReloadingEnd; }

	void SetMouseActive(bool _b);

	float GetCurMouseSensitivity() const { return m_MouseSensitivity; }
	bool IsShot() const { return m_bShoot; }
	bool IsThrow() const { return m_bCanThrow; }
	bool IsInventoryOpened() const { return m_InventoryOpened; }
	bool IsPlayerReloading() { return m_bReloading; }
	bool IsPlayerReloadingEnd() { return m_bReloadingEnd; }

	void TriggerHeal(ITEM_TYPE PHealType);
	void DamageCalcul(CGameObject* _AtkObj, CGameObject* _Weapon, float _Damage);	// 공격 피격 처리

	void PlusKillCount() { m_KillCounts += 1; }
	int GetKillCount() const { return m_KillCounts; }

	void SetMotionState(MOTION_STATE _State) { m_MotionState = _State; }
	void SetActionState(ACTION_STATE _State) { m_ActionState = _State; }

	MOTION_STATE GetMotionState() const { return m_MotionState; }
	ACTION_STATE GetActionState() const { return m_ActionState; }

	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;
	void LoadComponentReference() override;

public:
	CLONE(PlayerCharacter)
	PlayerCharacter();
	~PlayerCharacter() override;
};

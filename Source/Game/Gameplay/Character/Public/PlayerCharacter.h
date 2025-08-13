#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Engine/Runtime/Public/Component/StateMachine/CStateMachine.h"


class CGameObject;
class FLandscape;
class InteractionHandler;
class KillinfoUIScript;
class InventoryController;
class CameraController;
class CameraEffect;

enum class PLAYER_STATE
{
	Player_Dead,
	Player_Idle,
	Player_Jump,
	Player_Grenade_Prepare,
	Player_Grenade_Throw_High,
	Player_Grenade_Throw_Low,
	Player_Gun_Fire,
	Player_Gun_Reload,
	Player_Heal,
};

enum class MOTION_STATE : uint8_t
{
	STAND,	// 서 있음
	CROUCH,	// 앉아 있음
	PRONE,	// 엎드려 있음
};

class PlayerCharacter :
	public CScript
{
private:
	Ptr<FSound>		m_HitSound;
	Ptr<FSound>		m_FootstepSound;
	Ptr<FSound>		m_RunFootstepSound;

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

	bool m_bReloading;
	bool m_bThrowBoom;
	bool m_bHitSoundPlayed;
	bool m_bFirstFootStep;

	float m_HitSoundAccTime;
	float m_FootStepSoundAccTime;
	float m_StateAccTime;

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
	InteractionHandler*		m_InteractionScript;
	CameraEffect*			m_CameraEffect;

	// ======
	// Status
	// ======
	static constexpr float	m_MaxHP		= 100.f;	// 최대 체력
	static constexpr float	m_SemiMaxHP = 75.f;		// 붕대, 구급상자로 회복할 수 있는 최대 체력
	static constexpr float	m_MaxBoost	= 100.f;	// 최대 Boost
	static constexpr float	m_BoostUnit = 0.3f;		// 시간 지나면 boost가 빠질 단위

	float					m_CurHP;		// 현재 체력
	float					m_CurBoost;		// 현재 boost

	ITEM_TYPE				m_HealType;		// 힐 아이템 종류

	float					m_HealRemainTime;
	float					m_HealTotalTime;
	float					m_HealAmount;		// heal 또는 boost 양

	float					m_BoostRemainTime;	// boost 발생하는 시간 측정용 변수
	static constexpr float	m_BoostTotalTime = 8.f;	// boost가 수행되는 시간 단위 (8s)
	float					m_BoostSpeed;		// 부스트로 인한 이동속도 보정

	int						m_KillCounts;		// 킬 카운트

	MOTION_STATE			m_MotionState;

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

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;

private:
	void LoadPlayerSounds();

	//void PlayerMove();
	void PlayerView();
	void PlayerStance();

	void PlayerControlWeapon();

	void PlayerControlUI();
	void PlayerHeal();

	void InitMove();
	void UpdateMove();
	void UpdateGravity();
	void UpdateCollision();
	//void AnimationControl();

	bool CanRun();
	bool CanReload();

public:
	CGameObject* GetRayTarget() const { return m_CollObject; }

	void SetThrowBoom(bool PBoom) { m_bThrowBoom = PBoom; }
	void SetReloading(bool PReloading) { m_bReloading = PReloading; }
	void SetMouseActive(bool _b);
	void ResetAccTime() { m_StateAccTime = 0.f; }

	float GetCurMouseSensitivity() const { return m_MouseSensitivity; }
	bool IsInventoryOpened() const { return m_InventoryOpened; }

	bool IsGround() const { return m_IsGround; }

	// Heal
	void TriggerHeal(ITEM_TYPE PHealType);
	void DamageCalcul(CGameObject* _AtkObj, CGameObject* _Weapon, float _Damage);	// 공격 피격 처리
	void Heal();

	ITEM_TYPE GetHealType() const { return m_HealType; }

	void PlusKillCount() { m_KillCounts += 1; }
	int GetKillCount() const { return m_KillCounts; }

	void SetMotionState(MOTION_STATE _State) { m_MotionState = _State; }

	MOTION_STATE GetMotionState() const { return m_MotionState; }

	InventoryController* GetInventory() const { return m_InventoryScript; }

	Vec3 GetPlayerVelocity() const { return m_Velocity; }

	PLAYER_STATE GetStateEnum() const;
	

	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;
	void LoadComponentReference() override;

	// State Functions
	// Enter

	// FinalTick
	void ProgressHealState();
	void ProgressReloadState();
	void ProgressFireState();
	void ProgressThrowPrepareState(bool _InputThrow, bool _LBTN);
	void ProgressThrowState();
	void ProgressPlayerMove();
	

	// Exit
	void ExitThrowPrepareState();
	void ExitReloadState();

	

public:
	CLONE(PlayerCharacter)
	PlayerCharacter();
	~PlayerCharacter() override;
};

#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"
#include "Game/Gameplay/Inventory/Public/ItemMgr.h"

class CGameObject;
class CLandScape;


class PlayerCharacter :
	public CScript
{
private:
	CGameObject* m_MainCamera;

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
	bool m_bCanThrow;
	bool m_bThrowBoom;

	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

	CGameObject* m_CollObject;
	CGameObject* m_HeadColl;

	vector<Vec3> m_vecCollisionNormal; // 충돌 노말 벡터

	// ======
	// Script
	// ======
	class CameraController*		m_CamScript;
	class InventoryController*	m_InventoryScript;
	class KillinfoUIScript*		m_KillinfoScript;

	// ======
	// Status
	// ======
	const float		m_MaxHP;	// 최대 체력
	const float		m_SemiMaxHP;	// 붕대, 구급상자로 회복할 수 있는 최대치의 비율
	float			m_CurHP;	// 현재 체력

	const float		m_MaxBoost;	// 최대 Boost
	float			m_CurBoost;	// 에너지

	ITEM_TYPE		m_HealType;	// 힐 아이템 종류

	float			m_RemainTime;
	float			m_TotalTime;
	float			m_HealAmount;	// heal 또는 boost 양

	float			m_BoostRemainTime;
	const float		m_BoostTotalTime;	// boost가 수행되는 시간 단위
	const float		m_BoostUnit;	// 시간 지나면 boost가 빠질 단위
	float			m_BoostSpeed;	// 부스트로 인한 이동속도 보정

	int				m_KillCounts; // 킬 카운트

	// =======
	// UI 관리
	// =======
	CGameObject*	m_InventoryCanvasUI;	// 인벤토리 UI
	bool			m_InventoryOpened;

	CGameObject*	m_CardinalImageUI;		// 방위 UI
	CGameObject*	m_HPUI;					// HP UI
	CGameObject*	m_ItemUseUI;			// 아이템 사용 UI

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
	void PlayerStance();
	
	void PlayerReload();
	void PlayerAttack();

	void PlayerControlUI();
	void PlayerHeal();

	void MoveCalcul();
	void gravityCalcul();
	void ColliderCalcul();

	
	

public:
	CGameObject* GetRayTarget() const { return m_CollObject; }

	void SetShot(bool _Shot) { m_bShoot = _Shot; }
	void SetThrow(bool _Throw) { m_bCanThrow = _Throw; }
	void SetThrowBoom(bool _Boom) { m_bThrowBoom = _Boom; }

	float GetCurMouseSensitivity() const { return m_MouseSensitivity; }
	bool IsShot() const { return m_bShoot; }
	bool IsThrow() const { return m_bCanThrow; }
	bool IsInventoryOpened() const { return m_InventoryOpened; }

	void TriggerHeal(ITEM_TYPE _HealType);

	void DemageCalcul(CGameObject* _AtkObj, CGameObject* _Weapon, float _Damage);	// 공격 피격 처리

	void PlusKillCount() { m_KillCounts += 1; }
	int GetKillCount() { return m_KillCounts; }

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;
	void LoadComponentReference() override;

public:
	CLONE(PlayerCharacter);
	PlayerCharacter();
	~PlayerCharacter() override;
};

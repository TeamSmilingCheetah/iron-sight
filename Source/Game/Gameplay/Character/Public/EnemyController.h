#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

struct AItKeyInfo
{
	KEY_STATE State;
	bool Presssed;
	bool PrevPressed;
};

enum class Enemy_State
{
	Trace,
	Attack,
	Death,
	None,
};

#define AIKEY_CHECK(KEY, STATE) GetAIKeyState(KEY) == STATE
#define AIKEY_TAP(KEY) AIKEY_CHECK(KEY, KEY_STATE::TAP)
#define AIKEY_RELEASED(KEY) AIKEY_CHECK(KEY, KEY_STATE::RELEASED)
#define AIKEY_PRESSED(KEY) AIKEY_CHECK(KEY, KEY_STATE::PRESSED)

class EnemyController :
    public CScript
{
public:
	// Player
	CGameObject* m_Player;
	class PlayerCharacter* m_PlayerScript;
	class KillinfoUIScript* m_KillinfoScript;

	// 현재 사용중인 무기
	CGameObject*	m_CurWeapon;
	int				m_CurWeaponIdx;

	// 입력중인 키
	vector<AItKeyInfo> m_vecAIKey;

	// 시야
	CGameObject* m_VisionObj;
	class EnemyVisionScript* m_VisionScript;

	// 이동방향,크기 결정 값
	Vec3 m_InputMoveDir;
	float m_InputMoveForce;

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

	bool m_IsGround;				// 지상위에 서있는지 판정

	vector<Vec3> m_vecCollisionNormal; // 충돌 노말 벡터

	// 스텟
	float	m_HP;

	Enemy_State m_State;
	Enemy_State	m_PrevState;
	

public:
	void Begin() override;
	void Tick() override;

	virtual void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;
	virtual void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;
	virtual void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;


	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	virtual void DamageCalcul(CGameObject* _AtkObject, CGameObject* _Weapon, float _Damage);		// 피격 처리
	virtual void AttachItem(CGameObject* _Item, CGameObject* _BoneObject, Vec3 _RelativePos, Vec3 _RelativeRot);
	virtual void DetachItem(CGameObject* _Item);


	// 키입력 판정을 위한 함수
	void KeyPush(KEY _Key);						// 키 입력
	void KeyTick();								// 키 입력 정리

	virtual void KeyInputProcessing() {};		// 키 입력 처리(자식 클래스에서 사용)

	Enemy_State GetState() { return m_State; };

	KEY_STATE GetAIKeyState(KEY _Key) { return m_vecAIKey[static_cast<int>(_Key)].State; }

private:
	void UpdatePosition();
	void UpdateRotation();
	void UpdateMove();
	void UpdateGravity();
	void UpdateCollision();

public:
	EnemyController(SCRIPT_TYPE _Type);
	~EnemyController() override;
};

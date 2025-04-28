#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

enum class Enemy_State
{
	Trace,
	Attack,
	Death,
	None,
};

class EnemyController :
    public CScript
{
public:
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
	int	m_HP;

	Enemy_State m_State;
	Enemy_State	m_PrevState;
	

public:
	void Begin() override {};
	void Tick() override;

	virtual void BeginOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;
	virtual void Overlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;
	virtual void EndOverlap(CCollider3D* _Collider, CGameObject* _OtherObject,CCollider3D* _OtherCollider) override;


	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) override;

	virtual void DemageCalcul(int _Demage);

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

private:
	void UpdatePosition();
	void MoveCalcul();
	void gravityCalcul();
	void ColliderCalcul();

public:
	EnemyController(SCRIPT_TYPE _Type);
	~EnemyController() override;
};


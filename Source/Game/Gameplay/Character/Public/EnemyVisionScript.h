#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class EnemyVisionScript :
    public CScript
{
private:
	SCRIPT_TYPE m_TargetType;
	set<CScript*> m_SetScript;

	float		m_VisionRange;
	float		m_AtkRange;
	float		m_AtkRgmax;

	bool m_Vision;
	bool m_RayTarget;
	bool m_RayAtkRg;

public:
	void Begin() override;
	void Tick() override final;

	virtual void BeginOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, FCollider3D* _3DCollider) override;
	virtual void Overlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, FCollider3D* _3DCollider) override;
	virtual void EndOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, FCollider3D* _3DCollider) override;

	virtual void BeginOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void Overlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;
	virtual void EndOverlap(FCollider3D* _Collider, CGameObject* _OtherObject, FCollider3D* _OtherCollider) override;

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	void SetDetectType(SCRIPT_TYPE _Type) { m_TargetType = _Type; }

	bool IsTargetView() { return (m_Vision && m_RayTarget); }
	bool IsTargetRange() { return (m_Vision && m_RayAtkRg); }

	const set<CScript*>& GetTargetScript() { return m_SetScript; }

public:
	CLONE(EnemyVisionScript)
	EnemyVisionScript();
	~EnemyVisionScript() override;
};


#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class EnemyVisionScript :
    public CScript
{
private:
	SCRIPT_TYPE m_TargetType;
	CScript*	m_TargetScript;

	float		m_VisionRange;
	float		m_VisionAngle;
	float		m_AtkRange;
	float		m_AtkRgmax;

	bool m_Vision;
	bool m_RayTarget;
	bool m_RayAtkRg;

public:
	void Begin() override;
	void Tick() override final;

	void BeginOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void Overlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;
	void EndOverlap(IColliderBase* InCollider, IColliderBase* InOtherCollider) override;

	void VisionCheck();

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	void SetDetectType(SCRIPT_TYPE _Type) { m_TargetType = _Type; }

	bool IsTargetView() { return (m_Vision && m_RayTarget); }
	bool IsTargetRange() { return (m_Vision && m_RayAtkRg); }

	CScript* GetTargetScript() { return m_TargetScript; }

public:
	CLONE(EnemyVisionScript)
	EnemyVisionScript();
	~EnemyVisionScript() override;
};


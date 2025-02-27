#pragma once
#include "Runtime/Public/Component/Script/CScript.h"

class CTexture;

class CPlayerScript :
	public CScript
{
	float m_PlayerSpeed;
	float m_PaperBurnIntence;
	Ptr<CTexture> m_TargetTex;
	Ptr<CPrefab> m_Prefab;

public:
	void Begin() override;
	void Tick() override;
	void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	                  CCollider2D* _OtherCollider) override;
	void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	             CCollider2D* _OtherCollider) override;
	void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
	                CCollider2D* _OtherCollider) override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	CLONE(CPlayerScript);
	CPlayerScript();
	~CPlayerScript() override;
};

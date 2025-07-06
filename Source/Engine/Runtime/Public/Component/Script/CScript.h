#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CPrefab;
class CCollider2D;
class CCollider3D;
class CColliderRay;
class CMeshCollider;

struct tScriptParam
{
	SCRIPT_PARAM Param;
	string Desc;
	void* pData;
};

class CScript :
	public CComponent
{
private:
	const SCRIPT_TYPE m_ScriptType;
	vector<tScriptParam> m_vecScriptParam;

	static constexpr SCRIPT_TYPE m_ParentType = SCRIPT_TYPE::NONE;

public:
	SCRIPT_TYPE GetScriptType() const { return m_ScriptType; }
	virtual SCRIPT_TYPE GetParentScriptType() { return m_ParentType; }
	void AddScriptParam(tScriptParam PParam) { m_vecScriptParam.push_back(PParam); }
	const vector<tScriptParam>& GetScriptParam() { return m_vecScriptParam; }
	void Instantiate(Ptr<CPrefab> PPref, Vec3 PWorldPos, int PLayer);

	void Tick() override = 0;

	void FinalTick() final
	{
	}

	// ==============
	// Collider Event
	// ==============

	// XXX(KHJ): 컴포지션 패턴 적용 시도
	// Col2D - Col2D
	virtual void BeginOverlap(CCollider2D* PCollider, CGameObject* POtherObject, CCollider2D* POtherCollider)
	{
	}

	virtual void Overlap(CCollider2D* PCollider, CGameObject* POtherObject, CCollider2D* POtherCollider)
	{
	}

	virtual void EndOverlap(CCollider2D* PCollider, CGameObject* POtherObject, CCollider2D* POtherCollider)
	{
	}

	// Col3D - Col3D
	virtual void BeginOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	virtual void Overlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	virtual void EndOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	// Ray - Col3D
	virtual void BeginOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* _3DCollider)
	{
	}

	virtual void Overlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* _3DCollider)
	{
	}

	virtual void EndOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CCollider3D* _3DCollider)
	{
	}

	// Col3D - LandScape
	virtual void BeginOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	virtual void Overlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	virtual void EndOverlap(CCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	// Ray - LandScape
	virtual void BeginOverlap(CColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	virtual void Overlap(CColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	virtual void EndOverlap(CColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider)
	{
	}

	// MeshCollider - MeshCollider
	virtual void BeginOverlap(CMeshCollider* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
	{
	}

	virtual void Overlap(CMeshCollider* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
	{
	}

	virtual void EndOverlap(CMeshCollider* PCollider, CGameObject* POtherObject, CMeshCollider* POtherCollider)
	{
	}

	// MeshCollider - Col3D
	virtual void BeginOverlap(CMeshCollider* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	virtual void Overlap(CMeshCollider* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	virtual void EndOverlap(CMeshCollider* PCollider, CGameObject* POtherObject, CCollider3D* POtherCollider)
	{
	}

	// ========
	// UI Event
	// ========
	virtual void OnMouseClick()
	{
	}

	virtual void OnMouseRightClick()
	{
	}

	virtual void OnMouseHover()
	{
	}

	virtual PayLoad OnMouseBeginDrag() { return PayLoad{}; }

	virtual void OnMouseDrop(const PayLoad& PPayLoad)
	{
	}


	CScript(SCRIPT_TYPE PScriptType);
	~CScript() override;
};

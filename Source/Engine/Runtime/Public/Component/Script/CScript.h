#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Game/System/Public/GameplayManager.h"

class CPrefab;

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
	SCRIPT_TYPE m_ParentScriptType;
	vector<tScriptParam> m_vecScriptParam;

public:
	void SetParentScriptType(SCRIPT_TYPE _Type) { m_ParentScriptType = _Type; };
	SCRIPT_TYPE GetScriptType() { return m_ScriptType; }
	SCRIPT_TYPE GetParentScriptType() { return m_ParentScriptType; }
	void AddScriptParam(tScriptParam _Param) { m_vecScriptParam.push_back(_Param); }
	const vector<tScriptParam>& GetScriptParam() { return m_vecScriptParam; }
	void Instantiate(Ptr<CPrefab> _Pref, Vec3 _WorldPos, int _Layer);

	void Tick() override = 0;

	void FinalTick() final {}

	// ==============
	// Collider Event
	// ==============

	// Col2D - Col2D
	virtual void BeginOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
	virtual void Overlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
	virtual void EndOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}

	// Col3D - Col3D
	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) {}
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) {}
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) {}

	// Ray - Col3D
	virtual void BeginOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider) {}
	virtual void Overlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)	{}
	virtual void EndOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider) {}

	// Col3D - LandScape
	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}

	// Ray - LandScape
	virtual void BeginOverlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}
	virtual void Overlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}
	virtual void EndOverlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {}

	// ========
	// UI Event
	// ========
	virtual void OnMouseClick() {}
	virtual void OnMouseRightClick() {}
	virtual void OnMouseHover() {}
	virtual PayLoad OnMouseBeginDrag() { return PayLoad{}; }
	virtual void OnMouseDrop(const PayLoad& _PayLoad) {}


	CScript(SCRIPT_TYPE _ScriptType);
	~CScript() override;
};

#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

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
	const UINT m_ScriptType;
	vector<tScriptParam> m_vecScriptParam;

public:
	UINT GetScriptType() { return m_ScriptType; }
	void AddScriptParam(tScriptParam _Param) { m_vecScriptParam.push_back(_Param); }
	const vector<tScriptParam>& GetScriptParam() { return m_vecScriptParam; }
	void Instantiate(Ptr<CPrefab> _Pref, Vec3 _WorldPos, int _Layer);

	void Tick() override = 0;

	void FinalTick() final
	{
	}

	virtual void BeginOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject,
	                          CCollider2D* _OtherCollider)
	{
	};

	virtual void Overlap(class CCollider2D* _Collider, CGameObject* _OtherObject,
	                     CCollider2D* _OtherCollider)
	{
	};

	virtual void EndOverlap(class CCollider2D* _Collider, CGameObject* _OtherObject,
	                        CCollider2D* _OtherCollider)
	{
	};

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
	{
	};

	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
	{
	};

	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider)
	{
	};

	virtual void BeginOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
	{
	};

	virtual void Overlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
	{
	};

	virtual void EndOverlap(class CColliderRay* _RayCollider, CGameObject* _OtherObject, CCollider3D* _3DCollider)
	{
	};

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};

	virtual void BeginOverlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};
	virtual void Overlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};
	virtual void EndOverlap(class CColliderRay* _Collider, CGameObject* _OtherObject, CLandScape* _OtherCollider) {};

	CScript(UINT _ScriptType);
	~CScript() override;
};

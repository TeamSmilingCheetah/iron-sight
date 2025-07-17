#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

struct tScriptParam;

class CScript :
	public CComponent
{
private:
	const SCRIPT_TYPE MScriptType;
	vector<tScriptParam> MScriptParamVector;
	static constexpr SCRIPT_TYPE MParentType = SCRIPT_TYPE::NONE;

public:
	static void Instantiate(Ptr<CPrefab> PPrefab, const Vec3& PWorldPosition, int PLayer);
	SCRIPT_TYPE GetScriptType() const { return MScriptType; }
	void AddScriptParam(const tScriptParam& PParam) { MScriptParamVector.push_back(PParam); }

	void Tick() override = 0;

	void FinalTick() final
	{
	}

	/********************/
	/** Collider Event **/
	/********************/

	// XXX(KHJ): 컴포지션 패턴 적용 고려할 수 있는지
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

	// Col3D - MeshCollider
	virtual void BeginOverlap(CCollider3D* P3DCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
	{
	}

	virtual void Overlap(CCollider3D* P3DCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
	{
	}

	virtual void EndOverlap(CCollider3D* P3DCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
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

	// Ray - MeshCollider
	virtual void BeginOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
	{
	}

	virtual void Overlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
	{
	}

	virtual void EndOverlap(CColliderRay* PRayCollider, CGameObject* POtherObject, CMeshCollider* PMeshCollider)
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

	// MeshCollider - Ray
	virtual void BeginOverlap(CMeshCollider* PMeshCollider, CGameObject* POtherObject, CColliderRay* PRayCollider)
	{
	}

	virtual void Overlap(CMeshCollider* PMeshCollider, CGameObject* POtherObject, CColliderRay* PRayCollider)
	{
	}

	virtual void EndOverlap(CMeshCollider* PMeshCollider, CGameObject* POtherObject, CColliderRay* PRayCollider)
	{
	}

	/**************/
	/** UI Event **/
	/**************/

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

	// Getter & Setter
	virtual SCRIPT_TYPE GetParentScriptType() const { return MParentType; }
	const vector<tScriptParam>& GetScriptParam() const { return MScriptParamVector; }

	// Special Member Function
	CScript(SCRIPT_TYPE PScriptType);
	~CScript() override;
};

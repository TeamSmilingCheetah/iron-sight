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

    // 2D
    virtual void BeginOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void BeginOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void BeginOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void BeginOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void BeginOverlap(FCollider2D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // 3D
    virtual void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void BeginOverlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Mesh
    virtual void BeginOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void BeginOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void BeginOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void BeginOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void BeginOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Ray
    virtual void BeginOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void BeginOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void BeginOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void BeginOverlap(FColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}
	virtual void BeginOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

    // LandScape
    virtual void BeginOverlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void BeginOverlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void BeginOverlap(CLandScape* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void BeginOverlap(CLandScape* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

    // =================================================================
    // Overlap
    // =================================================================

    // 2D
    virtual void Overlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void Overlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void Overlap(FCollider2D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void Overlap(FCollider2D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void Overlap(FCollider2D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // 3D
    virtual void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void Overlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Mesh
    virtual void Overlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void Overlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void Overlap(FMeshCollider* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void Overlap(FMeshCollider* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void Overlap(FMeshCollider* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Ray
    virtual void Overlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void Overlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void Overlap(FColliderRay* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void Overlap(FColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}
	virtual void Overlap(FColliderRay* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

    // LandScape
    virtual void Overlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void Overlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void Overlap(CLandScape* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void Overlap(CLandScape* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

    // =================================================================
    // EndOverlap
    // =================================================================

    // 2D
    virtual void EndOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void EndOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void EndOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void EndOverlap(FCollider2D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void EndOverlap(FCollider2D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // 3D
    virtual void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void EndOverlap(FCollider3D* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Mesh
    virtual void EndOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void EndOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void EndOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void EndOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}
    virtual void EndOverlap(FMeshCollider* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}

    // Ray
    virtual void EndOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void EndOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void EndOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void EndOverlap(FColliderRay* PCollider, CGameObject* POtherObject, CLandScape* POtherCollider) {}
	virtual void EndOverlap(FColliderRay* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

    // LandScape
    virtual void EndOverlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider2D* POtherCollider) {}
    virtual void EndOverlap(CLandScape* PCollider, CGameObject* POtherObject, FCollider3D* POtherCollider) {}
    virtual void EndOverlap(CLandScape* PCollider, CGameObject* POtherObject, FMeshCollider* POtherCollider) {}
    virtual void EndOverlap(CLandScape* PCollider, CGameObject* POtherObject, FColliderRay* POtherCollider) {}

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

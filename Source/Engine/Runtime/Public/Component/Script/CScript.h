#pragma once
#include "Engine/Runtime/Public/Component/Base/Component.h"

class IColliderBase;
struct tScriptParam;

class CScript :
	public FComponent
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

	virtual void BeginOverlap(IColliderBase* InSelf, IColliderBase* InOther) {}
	virtual void Overlap(IColliderBase* InSelf, IColliderBase* InOther) {}
	virtual void EndOverlap(IColliderBase* InSelf, IColliderBase* InOther) {}

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

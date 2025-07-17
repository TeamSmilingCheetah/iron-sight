#include "pch.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

CScript::CScript(SCRIPT_TYPE PScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, MScriptType(PScriptType)
{
}

CScript::~CScript() = default;

void CScript::Instantiate(Ptr<CPrefab> PPrefab, const Vec3& PWorldPosition, int PLayer)
{
	if (PPrefab == nullptr)
	{
		return;
	}

	CGameObject* NewObject = PPrefab->Instantiate();

	NewObject->Transform()->SetRelativePos(PWorldPosition);
	CreateObject(NewObject, PLayer, false);
}

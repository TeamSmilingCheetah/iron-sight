#include "pch.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Asset/Prefab/CPrefab.h"

CScript::CScript(SCRIPT_TYPE _ScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_ScriptType(_ScriptType)
{
}

CScript::~CScript()
{
}

void CScript::Instantiate(Ptr<CPrefab> _Pref, Vec3 _WorldPos, int _Layer)
{
	if (nullptr == _Pref)
		return;

	CGameObject* pNewObject = _Pref->Instantiate();

	pNewObject->Transform()->SetRelativePos(_WorldPos);

	CreateObject(pNewObject, _Layer, false);
}

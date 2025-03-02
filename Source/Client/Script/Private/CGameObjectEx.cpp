#include "pch.h"
#include "Client/Script/Public/CGameObjectEx.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

void CGameObjectEx::FinalTick_Editor()
{
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (!GetComponent(static_cast<COMPONENT_TYPE>(i)))
			continue;

		GetComponent(static_cast<COMPONENT_TYPE>(i))->FinalTick();
	}

	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		auto pChild = dynamic_cast<CGameObjectEx*>(vecChild[i]);
		assert(pChild);
		pChild->FinalTick_Editor();
	}
}

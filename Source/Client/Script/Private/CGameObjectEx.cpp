#include "pch.h"
#include "Client/Script/Public/CGameObjectEx.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"

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
		// FIXME : CGameObject를 Ex로 downcasting해서 작동하는지 테스트. 변경 필요함
		auto pChild = static_cast<CGameObjectEx*>(vecChild[i]);
		assert(pChild);
		pChild->FinalTick_Editor();
	}
}

void CGameObjectEx::Render_Editor()
{
	// 활성화 해제시 비활성화
	if (!IsActive())
		return;

	// 렌더 컴포넌트가 있으면 렌더링
	if (GetRenderComponent() != nullptr)
	{
		GetRenderComponent()->Render();
	}

	// 자식 오브젝트들도 렌더링
	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		// FIXME : CGameObject를 Ex로 downcasting해서 작동하는지 테스트. 변경 필요함
		auto pChild = static_cast<CGameObjectEx*>(vecChild[i]);
		if (pChild)
		{
			pChild->Render_Editor();
		}
	}
}

#include "pch.h"
#include "Client/Script/Public/CGameObjectEx.h"
#include "Engine/Runtime/Public/Component/Base/Component.h"
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"
#include "Engine/Runtime/Public/Component/Light/CLight3D.h"

void CGameObjectEx::FinalTick_Editor()
{
	FinalTick_Editor_Recur(this);
}

void CGameObjectEx::Render_Editor()
{
	Render_Editor_Recur(this);
}

void CGameObjectEx::FinalTick_Editor_Recur(CGameObject* _GameObject)
{
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (!_GameObject->GetComponent(static_cast<COMPONENT_TYPE>(i)))
			continue;

		FinalTick_Component(_GameObject->GetComponent(static_cast<COMPONENT_TYPE>(i)));
	}

	const vector<CGameObject*>& vecChild = _GameObject->GetChild();
	for (auto* pChild : vecChild)
	{
		FinalTick_Editor_Recur(pChild);
	}
}

void CGameObjectEx::Render_Editor_Recur(CGameObject* _GameObject)
{
	// 활성화 해제시 비활성화
	if (!_GameObject->IsActive())
		return;

	// 렌더 컴포넌트가 있으면 렌더링
	if (_GameObject->GetRenderComponent() != nullptr)
	{
		_GameObject->GetRenderComponent()->Render();
	}

	// 자식 오브젝트들도 렌더링
	const vector<CGameObject*>& vecChild = _GameObject->GetChild();
	for (auto* pChild : vecChild)
	{
		Render_Editor_Recur(pChild);
	}
}

void CGameObjectEx::FinalTick_Component(FComponent* _Component)
{
	switch (_Component->GetType())
	{
	case COMPONENT_TYPE::LIGHT3D:
		static_cast<CLight3D*>(_Component)->CalculateTransform();
	break;
	default:
		_Component->FinalTick();
	break;
	}
}

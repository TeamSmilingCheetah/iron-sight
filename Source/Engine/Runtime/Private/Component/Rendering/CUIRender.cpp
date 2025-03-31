#include "pch.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"


CUIRender::CUIRender()
	: CRenderComponent(COMPONENT_TYPE::UIRENDER)
{
}

CUIRender::~CUIRender()
{
}

void CUIRender::FinalTick()
{
}

void CUIRender::Render()
{
	assert(UI());

	// 위치정보
	Transform()->Binding();

	UI()->Binding();

	// 메쉬의 모든 Subset(부위, 인덱스버퍼) 를 렌더링한다.
	for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		// 사용할 쉐이더
		GetMaterial(i)->Binding();

		// 렌더링
		GetMesh()->Render(i);
	}

	// Font Render
	UI()->FontRender();
}

void CUIRender::SaveComponent(FILE* _File)
{
}

void CUIRender::LoadComponent(FILE* _FILE)
{
}

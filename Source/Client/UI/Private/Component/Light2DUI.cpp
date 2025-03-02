#include "pch.h"
#include "Client/UI/Public/Component/Light2DUI.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

class CLight2D;

Light2DUI::Light2DUI()
	: ComponentUI("Light2DUI", COMPONENT_TYPE::LIGHT2D)
{
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::Render_Update()
{
	CLight2D* pLight2D = GetTargetObject()->Light2D();

	ComponentTitle("Light2D");
}

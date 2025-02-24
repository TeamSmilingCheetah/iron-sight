#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CLight2D.h>


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

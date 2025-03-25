#include "pch.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

CUI::CUI(UI_TYPE _uiType)
	: CComponent(COMPONENT_TYPE::UI)
	, m_UIType(_uiType)
	, m_Priority(1)
{
}

CUI::~CUI()
{
}

void CUI::SetPriority(int _Priority)
{
	assert(_Priority >= 1);

	m_Priority = _Priority;
	Vec3 vPos = Transform()->GetRelativePos();
	Transform()->SetRelativePos(vPos.x, vPos.y, static_cast<float>(_Priority));
}

void CUI::Begin()
{
	// CanvasUI일 경우만 등록.
	if (m_UIType == UI_TYPE::CANVAS)
	{
		// UI Manager에 등록
		CUIMgr::GetInst()->RegisterUI(this);
	}
}

void CUI::FinalTick()
{
	Vec2 vRes = CDevice::GetInst()->GetRenderResolution();
	Vec3 vPos = Transform()->GetWorldPos();
	Vec3 vScale = Transform()->GetWorldScale();
	m_LT.x = vPos.x - vScale.x / 2.f + vRes.x / 2.f;
	m_LT.y = -vPos.y - vScale.y / 2.f + vRes.y / 2.f;

	m_RB.x = vPos.x + vScale.x / 2.f + vRes.x / 2.f;
	m_RB.y = -vPos.y + vScale.y / 2.f + vRes.y / 2.f;
}

void CUI::SaveComponent(FILE* _File)
{
}

void CUI::LoadComponent(FILE* _File)
{
}

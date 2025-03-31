#include "pch.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Engine/System/Public/Manager/CFontMgr.h"

CUI::CUI(UI_TYPE _uiType)
	: CComponent(COMPONENT_TYPE::UI)
	, m_UIType(_uiType)
	, m_Priority(1)
{
}

CUI::~CUI()
{
}


void CUI::SetRectPos(Vec2 _Pos)
{
	Transform()->SetRelativePos(_Pos.x, _Pos.y, 0.f);
}

void CUI::SetRectPos(float _x, float _y)
{
	Transform()->SetRelativePos(_x, _y, 0.f);
}

Vec2 CUI::GetRectPos()
{
	Vec3 vPos = Transform()->GetRelativePos();
	return Vec2(vPos.x, vPos.y);
}

void CUI::SetRectSize(Vec2 _Size)
{
	Transform()->SetIndependentScale(true);
	Transform()->SetRelativeScale(_Size.x, _Size.y, 1.f);
}

void CUI::SetRectSize(float _x, float _y)
{
	Transform()->SetIndependentScale(true);
	Transform()->SetRelativeScale(_x, _y, 1.f);
}

Vec2 CUI::GetRectSize()
{
	Vec3 vScale = Transform()->GetRelativeScale();
	return Vec2(vScale.x, vScale.y);
}

void CUI::AddText(const wstring& _Text, float _x, float _y, float _FontSize, UINT _Color)
{
	m_TextInfo.push_back(FontRenderInfo{ _Text, Vec2(_x, _y), _FontSize, _Color });
}

void CUI::Binding()
{
	if (m_UIType == UI_TYPE::CANVAS)
	{
		// TEST : SCissor
		D3D11_RECT scissor = {};
		scissor.left = static_cast<LONG>(m_LT.x);
		scissor.top = static_cast<LONG>(m_LT.y);
		scissor.right = static_cast<LONG>(m_RB.x);
		scissor.bottom = static_cast<LONG>(m_RB.y);

		CONTEXT->RSSetScissorRects(1, &scissor);
	}
}

void CUI::FontRender()
{
	// Rasterizer Scissor Rect 계산 -> canvas에서만 수행
	static Vec4 CanvasRect;

	if (m_UIType == UI_TYPE::CANVAS)
	{
		CanvasRect.x = m_LT.x;
		CanvasRect.y = m_LT.y;
		CanvasRect.z = m_RB.x;
		CanvasRect.w = m_RB.y;
	}

	// Font Render
	for (const auto& info : m_TextInfo)
	{
		CFontMgr::GetInst()->DrawFontClipDirectly(info.Text, m_LT.x + info.Pos.x, m_LT.y + info.Pos.y, info.Size, info.Color, CanvasRect.x, CanvasRect.y, CanvasRect.z, CanvasRect.w);
	}
}

void CUI::Clear()
{
	Vec2 vRes = CDevice::GetInst()->GetRenderResolution();

	D3D11_RECT scissor = {};
	scissor.left = 0;
	scissor.top = 0;
	scissor.right = static_cast<LONG>(vRes.x);
	scissor.bottom = static_cast<LONG>(vRes.y);

	CONTEXT->RSSetScissorRects(1, &scissor);
}

void CUI::Begin()
{
	// CanvasUI일 경우만 등록.
	if (m_UIType == UI_TYPE::CANVAS)
	{
		// UI Manager에 등록
		CUIMgr::GetInst()->RegisterUI(this);
	}

	// CanvasUI가 아닌데 CanvasUI의 자식 계층에 속하지 않는 경우 UIMgr에서 처리할 수 없기 때문에 확인해줌
	else
	{
		CGameObject* pParent = GetOwner()->GetParent();

		while (pParent != nullptr)
		{
			if (pParent->UI() && pParent->UI()->m_UIType == UI_TYPE::CANVAS)
				break;
		}

		assert(pParent != nullptr);		// 끝까지 올라가도 찾을 수 없었다면
	}

	// 재질 등록
	UIRender()->GetDynamicMaterial(0)->SetScalarParam(VEC4_0, m_BackGroundColor);
	UIRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, m_Image);
}

void CUI::FinalTick()
{
	// Transform으로부터 LT와 RB를 역으로 계산
	// Transform component로 값을 바꾸는 경우도 있을 수 있기 때문에 일단 여기에 이런 식으로 구현함
	// 캡슐화를 잘 할 수 있다면 final tick 말고 position setter에서 역 로직 구현할 것.
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

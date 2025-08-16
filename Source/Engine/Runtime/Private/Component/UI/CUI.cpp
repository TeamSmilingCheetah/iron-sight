#include "pch.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/System/Public/Manager/CFontMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"

CUI::CUI()
	: FComponent(COMPONENT_TYPE::UI)
	, m_UIType(UI_DEFAULT)
	, m_Priority(0)
	, m_BackGroundColor(Vec4(0.f, 0.f, 0.f, 0.f))	// 기본값 : alpha 0
{
}

CUI::CUI(UINT _uiType)
	: FComponent(COMPONENT_TYPE::UI)
	, m_UIType(_uiType)
	, m_Priority(0)
	, m_BackGroundColor(Vec4(0.f, 0.f, 0.f, 0.f))	// 기본값 : alpha 0
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

void CUI::AddText(const wstring& _Text, float _posX, float _posY, UINT _FontSize, UINT _Color, const wstring& _Font)
{
	FontRenderInfo info{};
	info.Text = _Text;
	info.Pos.x = _posX;
	info.Pos.y = _posY;
	info.FontSize = _FontSize;
	info.Color = _Color;
	info.Font = _Font;

	m_TextInfo.push_back(info);
}

void CUI::ModifyText(int idx, const wstring& _Text, float _posX, float _posY, UINT _FontSize, UINT _Color, const wstring& _Font)
{
	m_TextInfo[idx].Text = _Text;
	m_TextInfo[idx].Pos.x = _posX;
	m_TextInfo[idx].Pos.y = _posY;
	m_TextInfo[idx].FontSize = _FontSize;
	m_TextInfo[idx].Color = _Color;
	m_TextInfo[idx].Font = _Font;
}

void CUI::Binding()
{
	if (m_UIType & UI_CANVAS)
	{
		D3D11_RECT scissor = {};
		scissor.left = static_cast<LONG>(m_LT.x);
		scissor.top = static_cast<LONG>(m_LT.y);
		scissor.right = static_cast<LONG>(m_RB.x);
		scissor.bottom = static_cast<LONG>(m_RB.y);

		CONTEXT->RSSetScissorRects(1, &scissor);
	}

	// TODO(Ssio) : Material Instance로 바꾸고 싶네
	// FIXME(Ssio): Pause하면 오류 나는 이유 여기서 GetDynamicMaterial()이 null을 반환해줌
	UIRender()->GetDynamicMaterial(0)->SetScalarParam(VEC4_0, m_BackGroundColor);
	UIRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, m_Image);
}

void CUI::FontRender()
{
	// Rasterizer Scissor Rect 계산 -> canvas에서만 수행
	static Vec4 CanvasRect;

	if (m_UIType & UI_CANVAS)
	{
		CanvasRect.x = m_LT.x;
		CanvasRect.y = m_LT.y;
		CanvasRect.z = m_RB.x;
		CanvasRect.w = m_RB.y;
	}

	// Font Render
	for (const auto& info : m_TextInfo)
	{
		if (info.Font == L"")
			CFontMgr::GetInst()->DrawFontClipDirectly(info.Text, m_LT.x + info.Pos.x, m_LT.y + info.Pos.y, info.FontSize, info.Color, CanvasRect, Vec2(m_RB.x - m_LT.x, m_RB.y - m_LT.y));
		else
			CFontMgr::GetInst()->DrawFontClipDirectly(info.Text, m_LT.x + info.Pos.x, m_LT.y + info.Pos.y, info.FontSize, info.Color, CanvasRect, Vec2(m_RB.x - m_LT.x, m_RB.y - m_LT.y), info.Font);
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
	// CanvasUI가 아닌데 CanvasUI의 자식 계층에 속하지 않는 경우 UIMgr에서 처리할 수 없기 때문에 확인해줌
	if (!(m_UIType & UI_CANVAS))
	{
		CGameObject* pParent = GetOwner()->GetParent();

		while (pParent != nullptr)
		{
			if (pParent->UI() && pParent->UI()->m_UIType & UI_CANVAS)
				break;

			pParent = pParent->GetParent();
		}

		assert(pParent != nullptr);		// 끝까지 올라가도 찾을 수 없었다면
	}

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

	// CanvasUI일 경우만 등록.
	if (m_UIType & UI_CANVAS)
	{
		// UI Manager에 등록
		CUIMgr::GetInst()->RegisterUI(this);
	}
}

void CUI::SaveComponent(FILE* _File)
{
	fwrite(&m_UIType, sizeof(UINT), 1, _File);
	fwrite(&m_BackGroundColor, sizeof(Vec4), 1, _File);
	SaveAssetRef(m_Image, _File);

	size_t TextCount = m_TextInfo.size();
	fwrite(&TextCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < TextCount; ++i)
	{
		// FontRenderInfo를 저장 (필요한 것만)
		SaveWString(m_TextInfo[i].Font, _File);					// 폰트
		fwrite(&m_TextInfo[i].FontSize, sizeof(UINT), 1, _File);	// 사이즈
		SaveWString(m_TextInfo[i].Text, _File);					// 텍스트
		fwrite(&m_TextInfo[i].Color, sizeof(Vec4), 1, _File);	// 색상
		fwrite(&m_TextInfo[i].Pos, sizeof(Vec2), 1, _File);		// 위치
	}
}

void CUI::LoadComponent(FILE* _File)
{
	fread(&m_UIType, sizeof(UINT), 1, _File);
	fread(&m_BackGroundColor, sizeof(Vec4), 1, _File);
	LoadAssetRef(m_Image, _File);

	size_t TextCount = 0;
	fread(&TextCount, sizeof(size_t), 1, _File);

	m_TextInfo.resize(TextCount);

	for (size_t i = 0; i < TextCount; ++i)
	{
		// FontRenderInfo를 저장 (필요한 것만)
		LoadWString(m_TextInfo[i].Font, _File);					// 폰트
		fread(&m_TextInfo[i].FontSize, sizeof(UINT), 1, _File);	// 사이즈
		LoadWString(m_TextInfo[i].Text, _File);					// 텍스트
		fread(&m_TextInfo[i].Color, sizeof(Vec4), 1, _File);	// 색상
		fread(&m_TextInfo[i].Pos, sizeof(Vec2), 1, _File);		// 위치
	}
}

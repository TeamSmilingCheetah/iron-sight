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

void CUI::SetPriority(float _Priority)
{
	//assert(_Priority >= 0.f);

	m_Priority = _Priority;
	Vec3 vPos = Transform()->GetRelativePos();

	if (m_UIType == UI_TYPE::CANVAS)
		Transform()->SetRelativePos(vPos.x, vPos.y, _Priority + 2.f);
	else
		Transform()->SetRelativePos(vPos.x, vPos.y, _Priority);
}

void CUI::SetRectPos(Vec2 _Pos)
{
	Vec3 vPos = Transform()->GetRelativePos();
	Transform()->SetRelativePos(_Pos.x, _Pos.y, vPos.z);
}

void CUI::SetRectPos(float _x, float _y)
{
	Vec3 vPos = Transform()->GetRelativePos();
	Transform()->SetRelativePos(_x, _y, vPos.z);
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
	MeshRender()->GetDynamicMaterial(0)->SetScalarParam(VEC4_0, m_BackGroundColor);
	MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, m_Image);
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

	for (const auto& info : m_TextInfo)
	{
		// UI 위치에 대한 상대적인 위치에 출력함.
		CFontMgr::GetInst()->DrawFont(info.Text, m_LT.x + info.Pos.x, m_LT.y + info.Pos.y, info.Size, info.Color);
	}
}

void CUI::SaveComponent(FILE* _File)
{
}

void CUI::LoadComponent(FILE* _File)
{
}

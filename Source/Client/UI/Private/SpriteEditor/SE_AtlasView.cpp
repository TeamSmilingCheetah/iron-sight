#include "pch.h"
#include "Client/UI/Public/SpriteEditor/SE_AtlasView.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

SE_AtlasView::SE_AtlasView()
	: SE_Sub("SE_AtlasView")
	  , m_WidthSize(200.f)
	  , m_WheelScale(1.f)
	  , m_Ratio(0.f)
{
}

SE_AtlasView::~SE_AtlasView()
{
}

void SE_AtlasView::Render_Update()
{
	if (nullptr == m_AtlasTex)
		return;

	WheelCheck();

	// 이미지
	auto uv_min = ImVec2(0.0f, 0.0f);
	auto uv_max = ImVec2(1.0f, 1.0f);

	auto tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	auto border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	// 실제 해상도 대비 출력 Image 의 비율
	m_Ratio = (m_WidthSize * m_WheelScale) / m_AtlasTex->GetWidth();

	ImGui::Image(m_AtlasTex->GetSRV().Get(),
	             ImVec2((m_WidthSize * m_WheelScale), m_AtlasTex->GetHeight() * m_Ratio)
	             , uv_min, uv_max, tint_col, border_col);

	// SelectCheck
	SelectCheck();

	// 선택한 영역에 사각형 그리기
	DrawSelectRect();
}

void SE_AtlasView::SetAtlasTex(Ptr<CTexture> _Tex)
{
	if (m_AtlasTex == _Tex)
		return;

	m_AtlasTex = _Tex;
	m_WidthSize = 200.f;
}

void SE_AtlasView::WheelCheck()
{
	if (0 < ImGui::GetIO().MouseWheel)
	{
		m_WheelScale += 0.05f;
	}

	if (0 > ImGui::GetIO().MouseWheel)
	{
		m_WheelScale -= 0.05f;
	}

	if (3.f < m_WheelScale)
		m_WheelScale = 3.f;
	if (m_WheelScale < 0.1f)
		m_WheelScale = 0.1f;
}

void SE_AtlasView::SelectCheck()
{
	// Image 위젯 좌상단 좌표
	ImageRectMin = ImGui::GetItemRectMin();
	float ArrImageMin[] = {ImageRectMin.x, ImageRectMin.y};
	ImGui::InputFloat2("ImageMin", ArrImageMin);

	// 현재 마우스 위치
	m_MousePos = ImGui::GetMousePos();
	float arrMousePos[] = {m_MousePos.x, m_MousePos.y};
	ImGui::InputFloat2("MousePos", arrMousePos);

	auto vDiff = ImVec2(m_MousePos.x - ImageRectMin.x, m_MousePos.y - ImageRectMin.y);
	vDiff = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);

	// 마우스 위치의 아틀라스 픽셀좌표
	float PixelPos[] = {vDiff.x, vDiff.y};
	ImGui::InputFloat2("PixelPos", PixelPos);

	// 마우스 왼쪽 Tap 체크
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_MouseLT = ImGui::GetMousePos();
		auto vDiff = ImVec2(m_MouseLT.x - ImageRectMin.x, m_MouseLT.y - ImageRectMin.y);
		m_MouseLT = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		m_MouseRB = ImGui::GetMousePos();
		auto vDiff = ImVec2(m_MouseRB.x - ImageRectMin.x, m_MouseRB.y - ImageRectMin.y);
		m_MouseRB = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
	}

	// 마우스 왼쪽 클릭 체크
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		auto vPixelPos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		auto vDiff = ImVec2(vPixelPos.x - ImageRectMin.x, vPixelPos.y - ImageRectMin.y);
		vPixelPos = Vec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);

		if (0.f <= vPixelPos.x && vPixelPos.x < m_AtlasTex->GetWidth()
			&& 0.f <= vPixelPos.y && vPixelPos.y < m_AtlasTex->GetHeight())
		{
			CalcSpriteSize(vPixelPos);
		}
	}
}

void SE_AtlasView::DrawSelectRect()
{
	auto MouseLTPos = ImVec2(m_MouseLT.x * m_Ratio + ImageRectMin.x,
	                         m_MouseLT.y * m_Ratio + ImageRectMin.y);
	auto MouseRBPos = ImVec2(m_MouseRB.x * m_Ratio + ImageRectMin.x,
	                         m_MouseRB.y * m_Ratio + ImageRectMin.y);

	ImGui::GetWindowDrawList()->AddRect(MouseLTPos, MouseRBPos
	                                    , ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f)), 0.f, 0,
	                                    1.f);
}

void SE_AtlasView::CalcSpriteSize(Vec2 _PixelPos)
{
	float left = m_AtlasTex->GetWidth() - 1.f;
	float top = m_AtlasTex->GetHeight() - 1.f;
	float right = 0.f;
	float bot = 0.f;

	// 등록아이디 검사용 Set Clear
	m_PixelID.clear();

	// Quene 에 클릭한 최초의 픽셀좌표 입력
	list<Vec2> queue;
	queue.push_back(_PixelPos);

	if (IsPixelOk(_PixelPos))
	{
		queue.push_back(_PixelPos);
		m_PixelID.insert(_PixelPos);
	}

	while (!queue.empty())
	{
		Vec2 vPixelPos = queue.front();
		queue.pop_front();

		if (vPixelPos.x < left)
			left = vPixelPos.x;
		if (vPixelPos.x > right)
			right = vPixelPos.x;
		if (vPixelPos.y < top)
			top = vPixelPos.y;
		if (vPixelPos.y > bot)
			bot = vPixelPos.y;


		// 주변 픽셀을 queue 에 넣는다.
		Vec2 vUp = vPixelPos + Vec2(0.f, 1.f); // 위
		Vec2 vDown = vPixelPos + Vec2(0.f, -1.f); // 아래
		Vec2 vLeft = vPixelPos + Vec2(-1.f, 0.f); // 좌
		Vec2 vRight = vPixelPos + Vec2(1.f, 0.f); // 우

		if (IsPixelOk(vUp))
		{
			queue.push_back(vUp);
			m_PixelID.insert(vUp);
		}

		if (IsPixelOk(vDown))
		{
			queue.push_back(vDown);
			m_PixelID.insert(vDown);
		}

		if (IsPixelOk(vLeft))
		{
			queue.push_back(vLeft);
			m_PixelID.insert(vLeft);
		}

		if (IsPixelOk(vRight))
		{
			queue.push_back(vRight);
			m_PixelID.insert(vRight);
		}
	}

	m_MouseLT = ImVec2(left, top);
	m_MouseRB = ImVec2(right, bot);
}

bool SE_AtlasView::IsPixelOk(Vec2 _PixelPos)
{
	// 해당 픽셀이 아틀라스 해상도 영역을 벗어난 좌표라면
	if (_PixelPos.x < 0 || m_AtlasTex->GetWidth() <= _PixelPos.x
		|| _PixelPos.y < 0 || m_AtlasTex->GetHeight() <= _PixelPos.y)
	{
		return false;
	}

	// 이미 등록된 적이 있는 Pixel 이라면
	if (m_PixelID.end() != m_PixelID.find(_PixelPos))
		return false;

	// 픽셀의 알파값으 0 이라면
	int PixelIdx = m_AtlasTex->GetWidth() * static_cast<int>(_PixelPos.y) + static_cast<int>(
		_PixelPos.x);
	if (0.f == m_AtlasTex->GetPixels()[PixelIdx].a)
		return false;

	return true;
}

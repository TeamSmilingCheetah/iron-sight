#include "pch.h"
#include "System/Public/Manager/CFontMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"

CFontMgr::CFontMgr()
	: m_FW1Factory(nullptr)
	  , m_FontWrapper(nullptr)
{
}

CFontMgr::~CFontMgr()
{
	if (nullptr != m_FW1Factory)
		m_FW1Factory->Release();

	if (nullptr != m_FontWrapper)
		m_FontWrapper->Release();
}

void CFontMgr::Init()
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_FW1Factory)))
	{
		assert(nullptr);
	}

	if (FAILED(m_FW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_FontWrapper)))
	{
		assert(nullptr);
	}
}

void CFontMgr::DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize,
						UINT _Color)
{
	// 컨테이너에 쌓아둠
	m_vecRenderInfo.push_back(FontRenderInfo{ _pStr, Vec2(_fPosX, _fPosY), _fFontSize, _Color });
}

void CFontMgr::Render()
{
	// 컨테이너에 쌓아둔 폰트 정보를 RenderMgr Render 이후 한 번에 처리
	for (const auto& info : m_vecRenderInfo)
	{
		m_FontWrapper->DrawString(
			CONTEXT,
			info.Text.c_str(), // String
			info.Size, // Font size
			info.Pos.x, // X position
			info.Pos.y, // Y position
			info.Color, // Text color, 0xAaBbGgRr
			FW1_RESTORESTATE // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
	}

	// 매 틱마다 등록해줘야 함.
	m_vecRenderInfo.clear();
}

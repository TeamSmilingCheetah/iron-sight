#include "pch.h"
#include "System/Public/Manager/CFontMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"

CFontMgr::CFontMgr()
	: m_FW1Factory(nullptr)
	, m_FontWrapper(nullptr)
	, m_WriteFactory(nullptr)
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

	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_WriteFactory)
	);

}

void CFontMgr::DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize,
						UINT _Color)
{
	Vec2 vRes = CDevice::GetInst()->GetRenderResolution();

	// 컨테이너에 쌓아둠
	m_vecRenderInfo.push_back(FontRenderInfo{ _pStr, Vec2(_fPosX, _fPosY), _fFontSize, _Color, Vec4(0.f, 0.f, vRes.x, vRes.y)});
}

void CFontMgr::DrawFontClip(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color, float _clipLeft, float _clipTop, float _clipRight, float _clipBottom)
{
	// 컨테이너에 쌓아둠
	m_vecRenderInfo.push_back(FontRenderInfo{ _pStr, Vec2(_fPosX, _fPosY), _fFontSize, _Color, Vec4(_clipLeft, _clipTop, _clipRight, _clipBottom) });
}

void CFontMgr::DrawFontClipDirectly(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color, float _clipLeft, float _clipTop, float _clipRight, float _clipBottom)
{
	IDWriteTextLayout* layout;

	IDWriteTextFormat* format = nullptr;

	HRESULT hr = m_WriteFactory->CreateTextFormat(
		L"Segoe UI",           // 폰트 이름
		nullptr,               // 폰트 컬렉션
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		_fFontSize,                 // 폰트 크기
		L"en-us",              // 로캘
		&format
	);

	if (FAILED(hr))
		assert(nullptr);

	hr = m_WriteFactory->CreateTextLayout(
		_pStr.c_str(),
		static_cast<UINT32>(_pStr.size()),
		format,
		300.0f,  // 레이아웃 너비
		50.0f,   // 레이아웃 높이
		&layout
	);

	if (FAILED(hr))
		assert(nullptr);

	const FW1_RECTF clipRect{ _clipLeft, _clipTop, _clipRight, _clipBottom };

	m_FontWrapper->DrawTextLayout(
		CONTEXT,
		layout,
		_fPosX,
		_fPosY,
		_Color,  // color
		&clipRect, // 여기서 clip rect 지정!
		nullptr,
		FW1_CLIPRECT
	);
}

void CFontMgr::Render()
{
	// 컨테이너에 쌓아둔 폰트 정보를 RenderMgr Render 이후 한 번에 처리
	for (const auto& info : m_vecRenderInfo)
	{
		//m_FontWrapper->DrawString(
		//	CONTEXT,
		//	info.Text.c_str(), // String
		//	info.Size, // Font size
		//	info.Pos.x, // X position
		//	info.Pos.y, // Y position
		//	info.Color, // Text color, 0xAaBbGgRr
		//	FW1_RESTORESTATE // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		//);

		IDWriteTextLayout* layout;

		IDWriteTextFormat* format = nullptr;

		HRESULT hr = m_WriteFactory->CreateTextFormat(
			L"Segoe UI",           // 폰트 이름
			nullptr,               // 폰트 컬렉션
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			info.Size,                 // 폰트 크기
			L"en-us",              // 로캘
			&format
		);

		if (FAILED(hr))
			assert(nullptr);

		hr = m_WriteFactory->CreateTextLayout(
			info.Text.c_str(),
			static_cast<UINT32>(info.Text.size()),
			format,
			300.0f,  // 레이아웃 너비
			50.0f,   // 레이아웃 높이
			&layout
		);

		if (FAILED(hr))
			assert(nullptr);

		const FW1_RECTF clipRect { info.Clip.x, info.Clip.y, info.Clip.z, info.Clip.w };

		m_FontWrapper->DrawTextLayout(
			CONTEXT,
			layout,
			info.Pos.x,
			info.Pos.y,
			info.Color,  // color
			&clipRect, // 여기서 clip rect 지정!
			nullptr,
			FW1_CLIPRECT
		);
	}

	// 매 틱마다 등록해줘야 함.
	m_vecRenderInfo.clear();
}

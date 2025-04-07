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

void CFontMgr::DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
	, Vec2 _Layout, const wstring& _Font)
{
	Vec2 vRes = CDevice::GetInst()->GetRenderResolution();

	// 컨테이너에 쌓아둠
	FontRenderInfo info{};
	info.Font = _Font;
	info.FontSize = _FontSize;
	info.Text = _pStr;
	info.Color = _Color;
	info.Pos = Vec2(_fPosX, _fPosY);
	info.Layout = _Layout;
	info.Clip = Vec4(0.f, 0.f, vRes.x, vRes.y);

	// 컨테이너에 쌓아둠
	m_vecRenderInfo.push_back(info);
}

void CFontMgr::DrawFontClip(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
	, Vec4 _Clip, Vec2 _Layout, const wstring& _Font)
{
	// 컨테이너에 쌓아둠
	FontRenderInfo info{};
	info.Font = _Font;
	info.FontSize = _FontSize;
	info.Text = _pStr;
	info.Color = _Color;
	info.Pos = Vec2(_fPosX, _fPosY);
	info.Layout = _Layout;
	info.Clip = _Clip;

	m_vecRenderInfo.push_back(info);
}

void CFontMgr::DrawFontClipDirectly(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
	, Vec4 _Clip, Vec2 _Layout, const wstring& _Font)
{
	IDWriteTextFormat* format = nullptr;
	IDWriteTextLayout* layout = nullptr;

	// fontsize를 wchar_t*로 변환
	wchar_t buff[5]{};
	swprintf_s(buff, L" %d", _FontSize);

	// Font 이름 + Font size로 구성된 Key
	wstring fontKey = _Font + buff;

	// Map에서 fontKey로 검색
	auto iter = m_mapFormat.find(fontKey);

	// 등록되어있지 않은 폰트라면 새로 만들어서 등록
	if (iter == m_mapFormat.end())
	{
		HRESULT hr = m_WriteFactory->CreateTextFormat(
			_Font.c_str(),				// 폰트 이름
			nullptr,					// 폰트 컬렉션
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			static_cast<float>(_FontSize),
			L"en-us",					// 로캘
			&format
		);

		if (FAILED(hr))
			assert(nullptr);

		// Map에 새로 만든 폰트를 삽입
		m_mapFormat.emplace(fontKey, format);
	}

	// 등록되어 있다면
	else
	{
		format = iter->second;
	}

	//HRESULT hr = m_WriteFactory->CreateTextFormat(
	//	_Font.c_str(),				// 폰트 이름
	//	nullptr,					// 폰트 컬렉션
	//	DWRITE_FONT_WEIGHT_REGULAR,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	1280,					// 폰트 크기
	//	L"en-us",					// 로캘
	//	&format
	//);

	const FW1_RECTF clipRect{ _Clip.x, _Clip.y, _Clip.z, _Clip.w };

	//Matrix matTrans
	//{
	//	2.f / 1280.f, 0.f, 0.f, 0.f,
	//	0.f, -2.f / 768.f, 0.f, 0.f,
	//	0.f, 0.f, 1.f, 0.f,
	//	-1.f, 1.f, 0.f, 1.f,
	//};

	//Vec4 right	 = XMVectorSet(g_Trans.matWorld._11, g_Trans.matWorld._12, g_Trans.matWorld._13, 0.0f);
	//Vec4 up		 = XMVectorSet(g_Trans.matWorld._21, g_Trans.matWorld._22, g_Trans.matWorld._23, 0.0f);
	//
	//float scaleX = right.Length();
	//float scaleY = up.Length();

	//// 정규화해서 스케일 제거
	//right = XMVector3Normalize(right);
	//up = XMVector3Normalize(up);

	//Matrix scaleFix = XMMatrixScaling(1.0f / scaleX, 1.0f / scaleY, 1.0f);

	//Matrix finalTransform = matTrans * scaleFix * g_Trans.matWVP;
	//Matrix finalTransform = matTrans * g_Trans.matWVP;

	//matTrans *= noScaleWorld * g_Trans.matView * g_Trans.matProj;

	//matTrans *= g_Trans.matWVP;

	/*if (finalTransform._43 < 0.f)
		finalTransform._43 = 0.f;

	Vec3 test = XMVector3TransformCoord(Vec3(_fPosX, _fPosY, 0.f), matTrans);*/

	// Text Layout을 생성
	m_WriteFactory->CreateTextLayout(
		_pStr.c_str(),
		static_cast<UINT32>(_pStr.size()),
		format,
		_Layout.x,	 // 레이아웃 너비
		_Layout.y,   // 레이아웃 높이
		&layout
	);

	// Text Layout을 출력
	m_FontWrapper->DrawTextLayout(
		CONTEXT,
		layout,
		_fPosX,
		_fPosY,
		_Color,  // color
		&clipRect, // 여기서 clip rect 지정!
		//nullptr,
		//(FLOAT*)finalTransform.m,
		nullptr,
		FW1_CLIPRECT
	);
}

void CFontMgr::Render()
{
	// 컨테이너에 쌓아둔 폰트 정보를 RenderMgr Render 이후 한 번에 처리
	for (const auto& info : m_vecRenderInfo)
	{
		IDWriteTextFormat* format = nullptr;
		IDWriteTextLayout* layout = nullptr;

		// fontsize를 wchar_t*로 변환
		wchar_t buff[5]{};
		swprintf_s(buff, L" %d", info.FontSize);

		// Font 이름 + Font size로 구성된 Key
		wstring fontKey = info.Font + buff;

		// Map에서 fontKey로 검색
		auto iter = m_mapFormat.find(fontKey);

		// 등록되어있지 않은 폰트라면 새로 만들어서 등록
		if (iter == m_mapFormat.end())
		{
			HRESULT hr = m_WriteFactory->CreateTextFormat(
				info.Font.c_str(),           // 폰트 이름
				nullptr,					 // 폰트 컬렉션
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				static_cast<float>(info.FontSize),
				L"en-us",					 // 로캘
				&format
			);

			if (FAILED(hr))
				assert(nullptr);

			// Map에 새로 만든 폰트를 삽입
			m_mapFormat.emplace(fontKey, format);
		}

		// 등록되어 있다면
		else
		{
			format = iter->second;
		}

		// Text Layout을 생성
		 m_WriteFactory->CreateTextLayout(
			info.Text.c_str(),
			static_cast<UINT32>(info.Text.size()),
			format,
			info.Layout.x,  // 레이아웃 너비
			info.Layout.y,   // 레이아웃 높이
			&layout
		);

		const FW1_RECTF clipRect { info.Clip.x, info.Clip.y, info.Clip.z, info.Clip.w };

		// Text Layout을 Clip해서 출력함
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

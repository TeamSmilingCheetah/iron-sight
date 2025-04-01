#pragma once
#include "Common/singleton.h"
#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>

#pragma comment(lib, "Dwrite.lib")

#ifdef _DEBUG
#pragma comment(lib, "FontEngine/FW1FontWrapper_debug.lib")
#else
#pragma comment(lib, "FontEngine/FW1FontWrapper.lib")
#endif


class CFontMgr :
	public singleton<CFontMgr>
{
	SINGLE(CFontMgr);

private:
	IFW1Factory*				m_FW1Factory;
	IFW1FontWrapper*			m_FontWrapper;
	IDWriteFactory*				m_WriteFactory;

	vector<FontRenderInfo>		m_vecRenderInfo;

	map<wstring, IDWriteTextFormat*>	m_mapFormat;

public:
	void Init();

	// 쌓아뒀다가 가장 마지막 SwapChain에 출력
	void DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
		, Vec2 _Layout = Vec2(300.f, 50.f), const wstring& _Font = L"Segoe UI");
	void DrawFontClip(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
		, Vec4 _Clip, Vec2 _Layout = Vec2(300.f, 50.f), const wstring& _Font = L"Segoe UI");

	// 바로 출력
	void DrawFontClipDirectly(const wstring& _pStr, float _fPosX, float _fPosY, UINT _FontSize, UINT _Color
		, Vec4 _Clip, Vec2 _Layout = Vec2(300.f, 50.f), const wstring& _Font = L"Segoe UI");


	void Render();
};

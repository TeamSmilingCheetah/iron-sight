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

public:
	void Init();

	// 쌓아뒀다가 가장 마지막 SwapChain에 출력
	void DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);
	void DrawFontClip(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color
			, float _clipLeft, float _clipTop, float _clipRight, float _clipBottom);

	// 바로 출력
	void DrawFontClipDirectly(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color
		, float _clipLeft, float _clipTop, float _clipRight, float _clipBottom);


	void Render();
};

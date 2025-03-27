#pragma once
#include "Common/singleton.h"
#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>

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
	IFW1Factory*			m_FW1Factory;
	IFW1FontWrapper*		m_FontWrapper;

	vector<FontRenderInfo>	m_vecRenderInfo;

public:
	void Init();
	void DrawFont(const wstring& _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);

	void Render();
};

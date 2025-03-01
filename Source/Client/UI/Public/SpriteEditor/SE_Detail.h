#pragma once
#include "UI/Public/SpriteEditor/SE_Sub.h"

class CTexture;

class SE_Detail :
	public SE_Sub
{
	Ptr<CTexture> m_AtlasTex;

	void Atlas();
	void AtlasInfo();

public:
	void SetAtlasTex(Ptr<CTexture> _Tex);

private:
	void SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

public:
	void Render_Update() override;

	SE_Detail();
	~SE_Detail() override;
};

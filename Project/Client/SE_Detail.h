#pragma once
#include "SE_Sub.h"

#include <Engine/CTexture.h>

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

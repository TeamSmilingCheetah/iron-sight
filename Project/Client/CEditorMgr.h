#pragma once
#include <Common/singleton.h>

class CGameObjectEx;

class CEditorMgr :
    public singleton<CEditorMgr>
{
    SINGLE(CEditorMgr);
    vector<CGameObjectEx*> m_vecEditorObj;

public:
    void Init();
    void Progress();
};

#pragma once


#include <Common/Ptr.h>
#include <Engine/assets.h>

#include "EditorUI.h"

class ParamUI
{
    static UINT g_ParamID;

public:
    static bool Param_Int(const string& _Desc, int* _Data, bool _Drag);
    static bool Param_Float(const string& _Desc, float* _Data, bool _Drag);
    static bool Param_Vec2(const string& _Desc, Vec2* _Data, bool _Drag);
    static bool Param_Vec4(const string& _Desc, Vec4* _Data, bool _Drag);

    static bool Param_Tex(const string& _Desc, Ptr<CTexture>& _Tex
                          , EditorUI* _Inst = nullptr, EUI_DELEGATE_2 _MemFunc = nullptr);

    static bool Param_Prefab(const string& _Desc, Ptr<CPrefab>& _Prefab
                             , EditorUI* _Inst = nullptr, EUI_DELEGATE_2 _MemFunc = nullptr);


    friend class CImGuiMgr;
};

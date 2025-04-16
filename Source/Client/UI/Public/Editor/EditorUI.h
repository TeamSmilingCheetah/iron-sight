#pragma once
#include "Client/imgui/imgui.h"

class EditorUI
{
    string m_Name;
    const string m_ID;

    EditorUI* m_ParentUI;
    vector<EditorUI*> m_vecChildUI;

    ImVec2 m_ChildSize;
    bool m_Active;
    bool m_Modal;
    bool m_UseMenuBar;
    bool m_Move;

public:
    void SetName(const string& _name) { m_Name = _name; }
    const string& GetName() { return m_Name; }
    const string& GetID() { return m_ID; }

    EditorUI* GetParentUI() { return m_ParentUI; }
    const vector<EditorUI*>& GetChildUI() { return m_vecChildUI; }

    ImVec2 GetChildSize() { return m_ChildSize; }
    void SetChildSize(ImVec2 _Size) { m_ChildSize = _Size; }

    EditorUI* AddChildUI(EditorUI* _Child)
    {
        m_vecChildUI.push_back(_Child);
        _Child->m_ParentUI = this;
        return _Child;
    }

    void SetActive(bool _bActive);
    bool IsActive() { return m_Active; }

    void SetModal(bool _Modal) { m_Modal = _Modal; }
    bool IsModal() { return m_Modal; }

    void SetMove(bool _Move) { m_Move = _Move; }
    bool IsMove() { return m_Move; }

    void UseMenuBar(bool _Use) { m_UseMenuBar = _Use; }
    bool IsUseMenuBar() { return m_UseMenuBar; }

    virtual void Tick();
    virtual void Render();
    virtual void Render_Update() = 0;

private:
    void Render_Modal();
    void Render_Modalless();
    void Render_Child();


    virtual void Activate()
    {
    }

    virtual void Deactivate()
    {
    };

public:
    EditorUI(const string& _ID);
    virtual ~EditorUI();
};


// 맴버함수 포인터타입 재정의(EidtorUI 의 맴버함수, 인자는 void, 반환타입 void)
using EUI_DELEGATE = void(EditorUI::*)(void);
using EUI_DELEGATE_1 = void(EditorUI::*)(DWORD_PTR);
using EUI_DELEGATE_2 = void(EditorUI::*)(DWORD_PTR, DWORD_PTR);

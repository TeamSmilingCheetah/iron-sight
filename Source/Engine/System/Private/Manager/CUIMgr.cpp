#include "pch.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

CUIMgr::CUIMgr()
	: m_HoverUI(nullptr)
	, m_FocusUI(nullptr)
	, m_FocusCanvas(nullptr)
	, m_DragUI(nullptr)
{

}

CUIMgr::~CUIMgr()
{

}

void CUIMgr::RegisterUI(CUI* _UI)
{
	assert(_UI->m_UIType & UI_CANVAS);

	bool pushed = false;

	// 넣을 자리가 비어있지 않았다면
	if (_UI->m_Priority >= m_vecUI.size() || m_vecUI[_UI->m_Priority] != nullptr)
	{
		// 1씩 더하면서 비어있지 않은 자리를 찾음
		while (_UI->m_Priority < m_vecUI.size() && m_vecUI[_UI->m_Priority] != nullptr)
		{
			++_UI->m_Priority;
		}

		// 공간이 충분하지 않다면
		if (_UI->m_Priority >= m_vecUI.size())
		{
			m_vecUI.resize(_UI->m_Priority + 1);
		}
	}

	m_vecUI[_UI->m_Priority] = _UI;
}

void CUIMgr::SwapPriority(CUI* _UI, int Priority)
{
	// 서로의 Priority를 교환해서 설정
	m_vecUI[Priority]->SetPriority(_UI->m_Priority);
	_UI->SetPriority(Priority);

	// vector에서도 자리 swap
	std::swap(m_vecUI[_UI->m_Priority], m_vecUI[Priority]);
}


void CUIMgr::OnMouseClick()
{
	const vector<CScript*>& vecScript = m_DragUI->GetOwner()->GetScripts();

	for (CScript* script : vecScript)
	{
		script->OnMouseClick();
	}
}

void CUIMgr::OnMouseRightClick()
{
	const vector<CScript*>& vecScript = m_DragUI->GetOwner()->GetScripts();

	for (CScript* script : vecScript)
	{
		script->OnMouseRightClick();
	}
}

void CUIMgr::OnMouseHover()
{
	const vector<CScript*>& vecScript = m_HoverUI->GetOwner()->GetScripts();

	for (CScript* script : vecScript)
	{
		script->OnMouseHover();
	}
}

void CUIMgr::OnMouseBeginDrag()
{
	m_vecPayLoad.clear();

	const vector<CScript*>& vecScript = m_DragUI->GetOwner()->GetScripts();

	for (CScript* script : vecScript)
	{
		m_vecPayLoad.push_back(script->OnMouseBeginDrag());
	}
}

void CUIMgr::OnMouseDrop()
{
	const vector<CScript*>& vecScript = m_HoverUI->GetOwner()->GetScripts();

	for (CScript* script : vecScript)
	{
		for (const PayLoad& payload : m_vecPayLoad)
			script->OnMouseDrop(payload);
	}
}

void CUIMgr::ChangeFocus(CUI* _CanvasUI, CUI* _FocusUI)
{
	// 이전 프레임 FocusCanvas랑 현재 Focus하려는 Canvas와 동일하다면
	if (_CanvasUI == m_FocusCanvas)
		return;

	m_FocusCanvas = _CanvasUI;
	m_FocusUI = _FocusUI;

	// 새로운 FocusCanvas가 있다면 -> Priority에 따라 Canvas 순서를 바꿈.
	if (m_FocusCanvas != nullptr)
	{
		int prevPriority = m_FocusCanvas->m_Priority;

		// FocusCanvas의 Priority을 0로 설정하고, 나머지는 한 칸씩 미룸.
		m_FocusCanvas->SetPriority(0);

		for (int i = prevPriority; i > 0; --i)
		{
			m_vecUI[i] = m_vecUI[i - 1];
			m_vecUI[i]->SetPriority(i);
		}

		m_vecUI[0] = m_FocusCanvas;
	}
}

void CUIMgr::Tick()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	if (pLevel == nullptr || pLevel->GetState() != LEVEL_STATE::PLAY)
		return;

	// Priority 정리 - nullptr인 부분 정리
	for (auto iter = m_vecUI.begin(); iter != m_vecUI.end(); )
	{
		if (*iter == nullptr)
			iter = m_vecUI.erase(iter);
		else
			++iter;
	}

	for (UINT i = 0; i < UINT(m_vecUI.size()); ++i)
	{
		m_vecUI[i]->SetPriority(i);
	}

	// 마우스 Event 감지
	m_HoverUI = nullptr;				// 실제로 Hover된 UI
	CUI* HoverCanvasUI = nullptr;		// HoverUI가 속한 Canvas
	UINT Priority = UINT_MAX;			// UI 우선순위

	// Canvas UI의 자식 오브젝트를 순회 (DFS)하면서 마우스가 겹치는 가장 자식 UI를 찾음
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		if (m_vecUI[i] == nullptr)
			continue;

		// Canvas Priority가 더 높은 경우 (더 뒤에 있는 경우) 걸러냄
		if (m_vecUI[i]->m_Priority > Priority)
			break;

		CUI* curUI = CheckMouseHover(m_vecUI[i]);

		// Hover된 UI를 찾은 경우
		if (curUI != nullptr)
		{
			Priority = m_vecUI[i]->m_Priority;
			m_HoverUI = curUI;
			HoverCanvasUI = m_vecUI[i];
		}
	}

	// =========
	// Event 처리
	// =========
	
	// HoverUI가 있다면 
	if (m_HoverUI)
	{
		// Hover Event 옵션이 있다면
		if (m_HoverUI->CanHover())
			OnMouseHover();
	}

	// 마우스 Down이 감지된 경우
	if (KEY_TAP(KEY::LBTN) || KEY_TAP(KEY::RBTN))
	{
		// Focus를 변경해줌
		ChangeFocus(HoverCanvasUI, m_HoverUI);

		// Drag 처리
		m_DragUI = m_HoverUI;

		// Drag 가능한 경우 BeginDrag 호출해서 PayLoad 를 받아둠.
		if (m_DragUI != nullptr && m_DragUI->CanDrag())
			OnMouseBeginDrag();
	}


	// 마우스 Up이 감지된 경우
	else if (KEY_RELEASED(KEY::LBTN) || KEY_RELEASED(KEY::RBTN))
	{
		// HoverUI가 없다면
		if (m_HoverUI == nullptr)
		{
			// Drag 기능을 종료함.
			m_DragUI = nullptr;
		}

		// HoverUI가 있다면
		else
		{
			// 1. Click -> 같은 Item에서 Down Up이 이루어졌다면
			if (m_HoverUI == m_DragUI)
			{
				// 1-1. DragUI가 Click 기능을 지원한다면
				if (m_DragUI->CanClick() && KEY_RELEASED(KEY::LBTN))
				{
					// Click 이벤트 호출
					OnMouseClick();
				}

				// 1-2. DragUI가 Right Click을 지원한다면
				else if (m_DragUI->CanRightClick() && KEY_RELEASED(KEY::RBTN))
				{
					// Right Click 이벤트 호출
					OnMouseRightClick();
				}

				// 1-2. DragUI가 Click 기능을 지원하지 않는다면
				else
				{
					// Drag 종료
					m_DragUI = nullptr;
				}
			}

			// 2. Drag Drop -> 서로 다른 Item에서 Down Up이 이루어졌다면
			else
			{
				// 2-1. DragUI가 Drag 기능을 지원하고 HoverUI가 Drop 기능을 지원한다면
				if (m_DragUI && m_DragUI->CanDrag() && m_HoverUI->CanDrop())
				{
					// Drop 이벤트 호출
					OnMouseDrop();
				}

				// 2-2. DragUI가 Drag 기능을 지원하지 않거나 HoverUI가 Drop 기능을 지원하지 않는다면
				else
				{
					// Drag 종료
					m_DragUI = nullptr;
				}
			}
		}
	}

	// 마우스 Pressed가 감지된 경우
	else if (KEY_PRESSED(KEY::LBTN) || KEY_PRESSED(KEY::RBTN))
	{
		// Drag가 이루어지는 중

		// Drag 중에 다른 Canvas에 hover 된다면 포커스를 변경해줌.
		ChangeFocus(HoverCanvasUI, m_HoverUI);
	}
}


CUI* CUIMgr::CheckMouseHover(CUI* _UI)
{
	if (!_UI->GetOwner()->IsActive())
		return nullptr;

	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// 해당 UI에 Hover되어 있다면
	if (_UI->m_LT.x <= vMousePos.x && vMousePos.x <= _UI->m_RB.x
		&& _UI->m_LT.y <= vMousePos.y && vMousePos.y <= _UI->m_RB.y)
	{
		vector<CGameObject*> vecChild = _UI->GetOwner()->GetChild();

		// 자식 UI에 대해 추가 검사 진행
		for (CGameObject* pChild : vecChild)
		{
			if (!pChild->UI())
				continue;

			CUI* childHoveredUI = CheckMouseHover(pChild->UI());

			if (childHoveredUI)
				return childHoveredUI;
		}

		// 자식 UI에 겹치지 않은 경우
		return _UI;
	}

	// 겹치는 UI가 없는 경우
	return nullptr;
}

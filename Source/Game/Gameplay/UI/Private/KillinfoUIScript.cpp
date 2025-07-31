#include "pch.h"
#include "Game/Gameplay/UI/Public/KillinfoUIScript.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"

#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"

using std::to_wstring;

KillinfoUIScript::KillinfoUIScript()
	: CScript(SCRIPT_TYPE::KILLINFOUI)
	, m_AccTime(0.f)
	, m_bEvent(false)
	, m_KilledName()
	, m_KillerName()
	, m_KillWeapon()
	, m_Player(nullptr)
	, m_PlayerScript(nullptr)
{
}

KillinfoUIScript::~KillinfoUIScript()
{
}

void KillinfoUIScript::Begin()
{
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	m_PlayerScript = static_cast<PlayerCharacter*>(GetScriptWithType(m_Player, SCRIPT_TYPE::PLAYERSCRIPT));
}

void KillinfoUIScript::Tick()
{

	if (m_bEvent)
	{
		// 만약 현재 UI TEXT가 비어있는 상태라면 만들어준다.
		if (UI()->GetTextvecSize() == 0)
		{
			UI()->AddText(L"", 11.f, 4.f, 25, FONT_RGBA(255, 255, 255, 255));
			UI()->AddText(L"", 30.f, 4.f, 30, FONT_RGBA(255, 255, 255, 255));
		}

		m_AccTime += DT;

		// Player가 사망한 경우
		if (m_KilledName == L"Player")
		{
			if (m_KillerName == L"Player")
				m_KillerName = L"당신";

			wstring killUI = m_KillerName + L"의 " + m_KillWeapon + L"로 인해 " + L"당신이 사망했습니다";
			UI()->ModifyText(0, killUI, 15.f, 4.f, 20, FONT_RGBA(255, 255, 255, 255));
		}
		else
		{
			wstring KillCounts = to_wstring(m_PlayerScript->GetKillCount()) + L" 킬";
			wstring killUI = L"당신의 " + m_KillWeapon + L"로 인해 " + m_KilledName + L"가 사망했습니다";

			// 만들어진 UI들을 수정하여 정보를 표시한다.
			UI()->ModifyText(0, killUI, 60.f, 4.f, 20, FONT_RGBA(255, 255, 255, 255));
			UI()->ModifyText(1, KillCounts, 205.f, 25.f, 35, FONT_RGBA(255, 0, 0, 255));
		}

		if (5.f < m_AccTime)
		{
			UI()->ClearText();
			m_bEvent = false;
			m_AccTime = 0.f;
		}
	}
}

void KillinfoUIScript::OnEvent()
{
	// 이벤트를 켜주는 것 뿐 아니라 시간 갱신 (중복 이벤트 발생시 갱신을 위함)
	m_AccTime = 0.f;
	m_bEvent = true;
}

void KillinfoUIScript::SaveComponent(FILE* _File)
{
}

void KillinfoUIScript::LoadComponent(FILE* _File)
{
}



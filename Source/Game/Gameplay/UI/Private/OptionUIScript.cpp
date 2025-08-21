#include "pch.h"

#include "Gameplay/UI/Public/OptionUIScript.h"


#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"

#include "Game/System/Public/CGameMgr.h"
#include "Game/Gameplay/Character/Public/PlayerCharacter.h"


OptionUIScript::OptionUIScript()
	: CScript(SCRIPT_TYPE::OPTIONMENUUI)
	, m_ClickCallback(nullptr)
	, m_ClickSound(nullptr)
	, m_ClickSoundIdx(-1)
{

}

OptionUIScript::OptionUIScript(function<void()> _Callback)
	: CScript(SCRIPT_TYPE::OPTIONMENUUI)
	, m_ClickCallback(_Callback)
	, m_ClickSound(nullptr)
	, m_ClickSoundIdx(-1)
{
}

OptionUIScript::~OptionUIScript()
{
}

void OptionUIScript::Begin()
{
	m_PlayerScript = CGameMgr::GetInst()->GetPlayerScript();

	// Sound
	m_ClickSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\menu_click.mp3");
}

void OptionUIScript::Tick()
{
	// CurSensi만 Modify 해야됨
	if (GetOwner()->UI()->CanDrag())
	{
		float curSensi = m_PlayerScript->GetCurMouseSensitivity();
		wstring strSensi = std::format(L"{:.2f}", curSensi);

		// 현재 Sensi Update
		UI()->ModifyText(1, strSensi, 90.f, 0.f, 20, FONT_RGBA(255, 255, 255, 255));
	}

	if (KEY_TAP(KEY::ESC))
	{
		SetObjectActive(CGameMgr::GetInst()->GetPauseCanvasUI(), true);
		SetObjectActive(CGameMgr::GetInst()->GetOptionCanvasUI(), false);
		m_PlayerScript->SetOptionUIOpened(false);
	}

}

void OptionUIScript::OnMouseClick()
{
	if (m_ClickCallback)
	{
		m_ClickCallback();
		Vec3 vPos = CGameMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();
		m_ClickSoundIdx = FSoundManager::GetInst()->Play3DSound(m_ClickSound, vPos, 1.f, 10000.f, 1, 1.f, false, false, m_ClickSoundIdx);
	}
}

void OptionUIScript::OnMouseBeginHover()
{
	GetOwner()->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 1.f));
	GetOwner()->UI()->SetTextColor(0, FONT_RGBA(0, 0, 0, 255));
}

void OptionUIScript::OnMouseEndHover()
{
	GetOwner()->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	GetOwner()->UI()->SetTextColor(0, FONT_RGBA(0, 0, 0, 188));
}

void OptionUIScript::ApplyOption()
{
}

void OptionUIScript::SaveComponent(FILE* _File)
{
}

void OptionUIScript::LoadComponent(FILE* _File)
{
}





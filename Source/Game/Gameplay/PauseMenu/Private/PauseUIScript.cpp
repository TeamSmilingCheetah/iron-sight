#include "pch.h"
#include "Game/Gameplay/PauseMenu/Public/PauseUIScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

#include "Game/System/Public/CGameMgr.h"

PauseUIScript::PauseUIScript()
	: CScript(SCRIPT_TYPE::PAUSEMENUUI)
	, m_ClickCallback(nullptr)
	, m_ClickSound(nullptr)
	, m_HoverSound(nullptr)
	, m_ClickSoundIdx(-1)
	, m_HoverSoundIdx(-1)
{
}

PauseUIScript::PauseUIScript(function<void()> _Callback)
	: CScript(SCRIPT_TYPE::PAUSEMENUUI)
	, m_ClickCallback(_Callback)
	, m_ClickSound(nullptr)
	, m_HoverSound(nullptr)
	, m_ClickSoundIdx(-1)
	, m_HoverSoundIdx(-1)
{

}

PauseUIScript::~PauseUIScript()
{
}

void PauseUIScript::Begin()
{
	// Sound
	m_HoverSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\menu_change.mp3");
	m_ClickSound = FAssetManager::GetInst()->Load<FSound>(L"Sound\\menu_click.mp3");
}

void PauseUIScript::Tick()
{
}

void PauseUIScript::OnMouseClick()
{
	if (m_ClickCallback)
	{
		m_ClickCallback();
		Vec3 vPos = CGameMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();
		m_ClickSoundIdx = FSoundManager::GetInst()->Play3DSound(m_ClickSound, vPos, 1.f, 10000.f, 1, 1.f, false, false, m_ClickSoundIdx);
	}
}

void PauseUIScript::OnMouseBeginHover()
{
	GetOwner()->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 1.f));
	GetOwner()->UI()->SetRectSize(330.f, 60.f);
	GetOwner()->UI()->SetTextSize(0, 35);
	GetOwner()->UI()->SetTextColor(0, FONT_RGBA(0, 0, 0, 255));
	Vec3 vPos = CGameMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();
	m_HoverSoundIdx = FSoundManager::GetInst()->Play3DSound(m_HoverSound, vPos, 1.f, 10000.f, 1, 1.f, true, true, -1);
}

void PauseUIScript::OnMouseEndHover()
{
	GetOwner()->UI()->SetColor(Vec4(0.8f, 0.8f, 0.8f, 0.5f));
	GetOwner()->UI()->SetRectSize(320.f, 50.f);
	GetOwner()->UI()->SetTextSize(0, 32);
	GetOwner()->UI()->SetTextColor(0, FONT_RGBA(0, 0, 0, 188));
}

void PauseUIScript::SaveComponent(FILE* _File)
{
}

void PauseUIScript::LoadComponent(FILE* _File)
{
}




#include "pch.h"
#include "Game/Gameplay/TestSound.h"

#include "Engine/System/Public/Manager/SoundManager.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

TestSound::TestSound()
	: CScript(SCRIPT_TYPE::TESTSOUND)
	, m_BulletSoundIdx(-1)
{
}

TestSound::~TestSound()
{
}

void TestSound::Begin()
{
	m_BulletSound = FAssetManager::GetInst()->Load<CSound>(L"Sound\\ak_reverb.wav", L"Sound\\ak_reverb.wav");

	Vec3 pPos = Transform()->GetRelativePos();
	m_BulletSoundIdx = FSoundManager::GetInst()->Play3DSound(m_BulletSound, pPos, 1.f, 10000.f, 1, 1.f, false, false, -1);
}

void TestSound::Tick()
{
	Vec3 pPos = Transform()->GetRelativePos();
	m_BulletSoundIdx = FSoundManager::GetInst()->Play3DSound(m_BulletSound, pPos, 1.f, 10000.f, 1, 1.f, true, false, m_BulletSoundIdx);
}

void TestSound::SaveComponent(FILE* _File)
{
}

void TestSound::LoadComponent(FILE* _File)
{
}

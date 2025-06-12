#include "pch.h"
#include "Game/Gameplay/Particle/Public/ParticleController.h"

#include "Engine/Runtime/Public/Component/Rendering/CParticleSystem.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

ParticleController::ParticleController()
	: CScript(SCRIPT_TYPE::PARTICLESCRIPT)
	, m_ParticleSystem(nullptr)
	, m_CurClipAccTime(0.f)
	, m_LifeTime(30.f)
	, m_SpawnTrigger(false)
{
}

ParticleController::~ParticleController()
{
}

void ParticleController::Begin()
{
	m_ParticleSystem = GetOwner()->ParticleSystem();
}

void ParticleController::Tick()
{
	// 파티클 시스템이 없으면 동작하지 않는다.
	if (m_ParticleSystem == nullptr)
		return;


	tParticleModule pModule = m_ParticleSystem->GetModule();
	pModule.Module[(UINT)PARTICLE_MODULE::SPAWN] = false;
	m_CurClipAccTime += DT;

	// 시간
	if (2.f < m_CurClipAccTime && !m_SpawnTrigger)
	{
		m_ParticleSystem->SetModule(pModule);
		m_SpawnTrigger = true;
	}

	if (m_LifeTime < m_CurClipAccTime)
	{
		DestroyObject(GetOwner());
		m_CurClipAccTime = 0.f;
	}

}

void ParticleController::SaveComponent(FILE* _File)
{
}

void ParticleController::LoadComponent(FILE* _File)
{
}




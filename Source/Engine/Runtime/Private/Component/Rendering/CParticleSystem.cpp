#include "pch.h"
#include "Runtime/Public/Component/Rendering/CParticleSystem.h"

#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/AssetManager.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CParticleSystem::CParticleSystem()
	: FRenderComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)
	  , m_ParticleBuffer(nullptr)
	  , m_SpawnCountBuffer(nullptr)
	  , m_ModuleBuffer(nullptr)
	  , m_Time(0.f)
	  , m_MaxParticleCount(1000)
{
	// Mesh / Material
	SetMesh(FAssetManager::GetInst()->FindAsset<CMesh>(L"PointMesh"));
	SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"ParticleMtrl"), 0);

	// ParticleTick ComputeShader
	m_TickCS = static_cast<CParticleTickCS*>(FAssetManager::GetInst()->FindAsset<CComputeShader>(
			L"ParticleTickCS").
		Get());

	// Particle Texture
	m_ParticleTex = FAssetManager::GetInst()->Load<CTexture>(
		L"Texture\\particle\\TX_GlowScene_2.png");

	// Particle Buffer
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SRV_UAV, false);

	// SpawnCount Buffer
	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SRV_UAV, true, nullptr);

	// ComputeShader
	m_TickCS = static_cast<CParticleTickCS*>(FAssetManager::GetInst()->FindAsset<CComputeShader>(
			L"ParticleTickCS").
		Get());


	// 파티클 기능(모듈) 정보 세팅
	// Spawn Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::SPAWN)] = true;
	m_Module.SpawnRate = 100;
	m_Module.vSpawnColor = Vec4(0.1f, 0.8f, 0.4f, 1.f);
	m_Module.MinLife = 1.f;
	m_Module.MaxLife = 5.f;
	m_Module.vSpawnMinScale = Vec3(5.f, 5.f, 1.f);
	m_Module.vSpawnMaxScale = Vec3(50.f, 50.f, 1.f);

	m_Module.SpawnShape = 1;
	m_Module.SpawnShapeScale.x = 800.f;

	m_Module.BlockSpawnShape = 1;
	m_Module.BlockSpawnShapeScale.x = 760.f;

	m_Module.SpaceType = 1; // Local Space

	// Spawn Burst Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::SPAWN_BURST)] = false;
	m_Module.SpawnBurstRepeat = true;
	m_Module.SpawnBurstCount = 100;
	m_Module.SpawnBurstRepeatTime = 3.f;

	// Scale Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::SCALE)] = true;
	m_Module.StartScale = 2.f;
	m_Module.EndScale = 2.f;

	// AddVelocity Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::ADD_VELOCITY)] = false;
	m_Module.AddVelocityType = 2;
	m_Module.AddVelocityFixedDir = Vec3(0.f, 1.f, 0.f);
	m_Module.AddMinSpeed = 100.f;
	m_Module.AddMaxSpeed = 500.f;

	// Drag Module (감속)
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::DRAG)] = false;
	m_Module.DestNormalizedAge = 0.2f;
	m_Module.LimitSpeed = 0.f;

	// Noise Force Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::NOISE_FORCE)] = true;
	m_Module.NoiseForceTerm = 0.3f;
	m_Module.NoiseForceScale = 200.f;

	// Render Module
	m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::RENDER)] = true;
	m_Module.EndColor = Vec3(0.8f, 0.1f, 0.5f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.5f;
	m_Module.VelocityAlignment = false;

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SRV_UAV, true, &m_Module);
}

CParticleSystem::CParticleSystem(const CParticleSystem& _Origin)
	: FRenderComponent(_Origin)
	  , m_TickCS(_Origin.m_TickCS)
	  , m_ParticleBuffer(nullptr)
	  , m_SpawnCountBuffer(nullptr)
	  , m_ModuleBuffer(nullptr)
	  , m_ParticleTex(_Origin.m_ParticleTex)
	  , m_Time(0.f)
	  , m_BurstTime(0.f)
	  , m_MaxParticleCount(_Origin.m_MaxParticleCount)
	  , m_Module(_Origin.m_Module)
{
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SRV_UAV, false);

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SRV_UAV, true, nullptr);

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SRV_UAV, true, &m_Module);
}

CParticleSystem::~CParticleSystem()
{
	delete m_ParticleBuffer;
	delete m_SpawnCountBuffer;
	delete m_ModuleBuffer;
}

void CParticleSystem::CaculateSpawnCount()
{
	m_Time += EngineDT;
	tSpawnCount count = {};

	if (m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::SPAWN)])
	{
		// SpawnRate 에 따른 이번 Tick 에서 생성시킬 파티클의 수
		float Term = 1.f / static_cast<float>(m_Module.SpawnRate);
		UINT SpawnCount = 0;

		if (Term < m_Time)
		{
			float Value = m_Time / Term;
			SpawnCount = static_cast<UINT>(Value);
			m_Time -= static_cast<float>(SpawnCount) * Term;
		}

		count.SpawnCount = SpawnCount;
	}

	if (m_Module.Module[static_cast<UINT>(PARTICLE_MODULE::SPAWN_BURST)])
	{
		UINT BurstCount = 0;

		if (0.f == m_BurstTime)
		{
			BurstCount = m_Module.SpawnBurstCount;
		}

		m_BurstTime += EngineDT;

		if (m_Module.SpawnBurstRepeat && m_Module.SpawnBurstRepeatTime <= m_BurstTime)
		{
			m_BurstTime -= m_Module.SpawnBurstRepeatTime;
			BurstCount += m_Module.SpawnBurstCount;
		}

		count.SpawnCount += BurstCount;
	}

	// SpawnCount 를 Buffer 에 전달
	m_SpawnCountBuffer->SetData(&count);
}

void CParticleSystem::FinalTick()
{
	// SpawnCount 계산
	CaculateSpawnCount();

	// ComputeShader
	m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCount(m_SpawnCountBuffer);
	m_TickCS->SetModuleBuffer(m_ModuleBuffer);

	m_TickCS->Execute();
}

void CParticleSystem::Render()
{
	// 위치정보 바인딩
	Transform()->Binding();

	// 파티클 버퍼 바인딩
	m_ParticleBuffer->Binding(16); // t16

	// 모듈 버퍼 바인딩
	m_ModuleBuffer->SetData(&m_Module);
	m_ModuleBuffer->Binding(17); // t17

	// 재질정보 바인딩
	GetMaterial(0)->SetTexParam(TEX_0, m_ParticleTex);
	GetMaterial(0)->Binding();

	// 렌더링
	GetMesh()->Render_Cluster_Instancing(m_MaxParticleCount);

	// 파티클 버퍼 바인딩 해제
	m_ParticleBuffer->Clear(16);
	m_ModuleBuffer->Clear(17);
}

void CParticleSystem::SaveComponent(FILE* _File)
{
	SaveAssetRef(m_TickCS, _File);
	SaveAssetRef(m_ParticleTex, _File);

	fwrite(&m_Time, sizeof(float), 1, _File);
	fwrite(&m_BurstTime, sizeof(float), 1, _File);
	fwrite(&m_MaxParticleCount, sizeof(int), 1, _File);
	fwrite(&m_Module, sizeof(tParticleModule), 1, _File);
}

void CParticleSystem::LoadComponent(FILE* _File)
{
	m_TickCS = static_cast<CParticleTickCS*>(LoadAssetRef<CComputeShader>(_File));
	LoadAssetRef(m_ParticleTex, _File);

	fread(&m_Time, sizeof(float), 1, _File);
	fread(&m_BurstTime, sizeof(float), 1, _File);
	fread(&m_MaxParticleCount, sizeof(int), 1, _File);
	fread(&m_Module, sizeof(tParticleModule), 1, _File);
}

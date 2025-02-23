#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CParticleTickCS.h"

class CStructuredBuffer;

struct tSpawnCount
{
    int SpawnCount;
    int Padding[3];
};

class CParticleSystem :
    public CRenderComponent
{
    Ptr<CParticleTickCS> m_TickCS;
    CStructuredBuffer* m_ParticleBuffer; // ��� ��ƼŬ ����
    CStructuredBuffer* m_SpawnCountBuffer; // ��ƼŬ Ȱ��ȭ ���� ���޿� ����
    CStructuredBuffer* m_ModuleBuffer; // Module Data Buffer

    Ptr<CTexture> m_ParticleTex;
    float m_Time; // �����ð�    
    float m_BurstTime; // SpawnBurst üũ�� ����

    int m_MaxParticleCount; // ��ƼŬ �ִ� ����

    tParticleModule m_Module; // ��ƼŬ�� ��� ����

public:
    void SetParticleTexture(Ptr<CTexture> _Tex) { m_ParticleTex = _Tex; }

private:
    void CaculateSpawnCount();

public:
    void FinalTick() override;
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _FILE) override;

    CLONE(CParticleSystem);
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _Origin);
    ~CParticleSystem() override;
};

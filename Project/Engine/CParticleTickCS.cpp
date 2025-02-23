#include "pch.h"
#include "CParticleTickCS.h"

#include "CStructuredBuffer.h"
#include "CAssetMgr.h"


CParticleTickCS::CParticleTickCS()
    : CComputeShader(L"Shader\\particle_tick.fx", "CS_ParticleTick", 1024, 1, 1)
      , m_ParticleBuffer(nullptr)
{
    m_NoiseTex = CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\noise\\noise_03.jpg",
                                                      L"Texture\\noise\\noise_03.jpg");
}

CParticleTickCS::~CParticleTickCS()
{
}

int CParticleTickCS::Binding()
{
    if (nullptr == m_ParticleBuffer || nullptr == m_SpawnCountBuffer)
        return E_FAIL;

    // ��ƼŬ ���� u0 �� ���ε�
    m_ParticleBuffer->Binding_CS_UAV(0);

    // ���� ���� ���� u1 �� ���ε�
    m_SpawnCountBuffer->Binding_CS_UAV(1);

    // Noise Texture t20 ���ε�
    m_NoiseTex->Binding_SRV_CS(20);

    // ��� ������ t21 �� ���ε�
    m_ModuleBuffer->Binding_CS_SRV(21);


    // ��ƼŬ ��� ����(��ƼŬ ��)
    m_Const.iArr[0] = static_cast<int>(m_ParticleBuffer->GetElementCount());

    // ��ƼŬ ���� ������Ʈ �߽� ��ġ
    m_Const.v4Arr[0] = m_ParticleWorldPos;

    return S_OK;
}

void CParticleTickCS::CalcGroupCount()
{
    m_GroupX = m_ParticleBuffer->GetElementCount() / m_GroupPerThreadX;
    if (m_ParticleBuffer->GetElementCount() % m_GroupPerThreadX)
        m_GroupX += 1;

    m_GroupY = 1;
    m_GroupZ = 1;
}

void CParticleTickCS::Clear()
{
    m_ParticleBuffer->Clear_CS_UAV(0);
    m_ParticleBuffer = nullptr;

    m_SpawnCountBuffer->Clear_CS_UAV(1);
    m_SpawnCountBuffer = nullptr;

    m_NoiseTex->Clear_SRV_CS();

    m_ModuleBuffer->Clear_CS_SRV(21);
    m_ModuleBuffer = nullptr;
}

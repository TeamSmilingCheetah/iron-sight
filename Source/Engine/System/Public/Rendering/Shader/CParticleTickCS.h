#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"

class CStructuredBuffer;
class CTexture;

class CParticleTickCS :
    public CComputeShader
{
    CStructuredBuffer* m_ParticleBuffer;
    CStructuredBuffer* m_SpawnCountBuffer;
    CStructuredBuffer* m_ModuleBuffer;

    Ptr<CTexture> m_NoiseTex;
    Vec3 m_ParticleWorldPos;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer) { m_ParticleBuffer = _Buffer; }
    void SetSpawnCount(CStructuredBuffer* _Buffer) { m_SpawnCountBuffer = _Buffer; }
    void SetModuleBuffer(CStructuredBuffer* _ModuleBuffer) { m_ModuleBuffer = _ModuleBuffer; }

    void SetParticleWorldPos(Vec3 _Pos) { m_ParticleWorldPos = _Pos; }
    void SetNoiseTexture(Ptr<CTexture> _Noise) { m_NoiseTex = _Noise; }

private:
    int Binding() override;
    void CalculateGroupCount() override;
    void Clear() override;

public:
    CParticleTickCS();
    ~CParticleTickCS() override;
};

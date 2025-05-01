#ifndef _PARTICLE_COMPOSITE
#define _PARTICLE_COMPOSITE

#include "value.fx"


struct VS_IN
{
    float3 vPos : POSITION; // 모델 공간 정점 (-0.5~+0.5)
    float2 vUV : TEXCOORD; // UV (0~1)
};

struct VS_OUT
{
    float4 vPosition : SV_Position; // NDC
    float2 vUV : TEXCOORD;
};

VS_OUT VS_ParticleComposite(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0;

    // RectMesh(–0.5~+0.5)를 NDC(–1~+1)로 매핑
    output.vPosition = float4(_in.vPos.xy * 2.0f, 0.0f, 1.0f);
    output.vUV = _in.vUV;

    return output;
}


float4 PS_ParticleComposite(VS_OUT _in) : SV_Target
{
    // t0 슬롯에 바인딩된 ParticleTargetTex 샘플링
    float4 col = g_tex_0.Sample(g_sam_1, _in.vUV);

    // 알파 그대로 사용 (이미 ParticleBuffer에서 알파가 세팅된 상태)
    return col;
}

#endif

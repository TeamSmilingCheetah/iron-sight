#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float4 vUV : TEXCOORD;    
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;    
    
    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBirnormal : BINORMAL;
    
};

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);    
    output.vUV = _in.vUV;
    
    // LocalPos 를 ViewPos 로 변환
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    
    // LocalSpace Normal(법선)방향 정보를 월드 기준으로 변경
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBirnormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{    
    float3 vColor = float3(0.7f, 0.7f, 0.7f);
    float3 vNormal = _in.vViewNormal;
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
        
    if (g_btex_1)
    {
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);        
        
        // 추출한 색상값(0~1 범위) 을 방향벡터 값의 범위(-1 ~ 1) 로 변경한다.
        vNormal = (vNormal * 2.f) - 1.f;
        
        // 탄젠트 공간(표면공간) 에 있는 각 축이, 
        // 적용시킬 표면의 각 방향이 될 수 있도록 하는 회전행렬
        float3x3 matRot =
        {
            _in.vViewTangent,           
            _in.vViewBirnormal,
            _in.vViewNormal,
        };

        vNormal = normalize(mul(vNormal, matRot));
    }
        
    // 광원 계산
    float3 vLightColor = (float3) 0.f;
    float3 vSpecuPow = (float3) 0.f;    
    
    for (int i = 0; i < g_Light3DCount; ++i)
    {
        CalcLight3D(i, _in.vViewPos, vNormal, vLightColor, vSpecuPow);
    }
           
    // 물체의 색상에 광원을 적용시킨다.
    vColor = vColor * vLightColor + vSpecuPow;
    return float4(vColor, 1.f);
}

#endif
#ifndef _DECAL
#define _DECAL

#include "value.fx"

// ===========================
// DecalShader
// MRT          : DECAL
// Mesh         : CubeMesh
// Rasterizer   : CULL_FRONT (카메라가 볼륨메쉬 내부로 진입했을 경우 대비)
// DepthStencil : NO_TEST_NO_WRITE
// BlendState   : AlphaBlend
// Domain       : DOMAIN_DECAL
// Parameter
#define MatInv          g_mat_0
#define POSITION_TARGET g_tex_0
#define OutputTex       g_tex_1
#define HasOutputTex    g_btex_1
#define AsLight         g_int_0
#define GlobalAlpha     g_float_0
// ============================

struct VS_IN
{
    float3 vPos : POSITION;    
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);    
    
    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target;
    float4 vEmissive : SV_Target1;
};


PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vSampleColor = (float4) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;
    
    float4 vViewPos = POSITION_TARGET.Sample(g_sam_1, vScreenUV);
    
    if (vViewPos.a == 0.f)    
        discard;
    
    // 로컬 공간에서 CubeMesh 내부인지 테스트
    float3 vLocalPos = abs(mul(float4(vViewPos.xyz, 1.f), MatInv).xyz);    
    if (vLocalPos.x > 0.5f || vLocalPos.y > 0.5f || vLocalPos.z > 0.5f)    
        discard;
 
    if (HasOutputTex)
    {
        vSampleColor = OutputTex.Sample(g_sam_0, float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.z + 0.5f)));       
    }     
    else
        vSampleColor = float4(0.f, 1.f, 0.f, 1.f);
        
    if(AsLight)
    {
        vSampleColor.rgb *= vSampleColor.a * 1.f;
        output.vEmissive = vSampleColor;
    }
    
    else
    {        
        output.vColor = vSampleColor;
        output.vColor.a *= GlobalAlpha;        
    }
    
    
    return output;
}


#endif
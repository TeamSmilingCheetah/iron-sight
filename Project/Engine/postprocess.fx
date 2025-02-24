#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"

// Vertex Shader 
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// Mesh : RectMesh

VS_OUT VS_Post(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}

float4 PS_Post(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
            
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);    
    float Aver = (vColor.r + vColor.g + vColor.b) / 3.f;    
    vColor.rgb = Aver;
        
    return vColor;
}



// =================
// Distortion Shader
// =================
VS_OUT VS_Distortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
        
    return output;
}

// 1. 픽셀쉐이더의 픽셀 좌표
// 2. 렌더 타겟 해상도

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    // SV_Position -> Pixelshader (픽셀좌표)
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;    
    
    // NoiseTexture 가 있으면
    if(g_btex_1)
    {
        float4 vNoise = g_tex_1.Sample(g_sam_0, vScreenUV + 0.05f * g_Time);
        vNoise -= 0.5f;    
        vNoise *= 0.05f;        
        vScreenUV += vNoise.xy;
    }
    
    vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    
    return vColor;
}



struct VS_VortexOut
{
    float4 vPosition : SV_Position;
    float2 vObjectUV : TEXCOORD;
};


VS_VortexOut VS_Vortex(VS_IN _in)
{
    VS_VortexOut output = (VS_VortexOut) 0.f;
            
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
        
    float4 vProjPos = mul(float4(0.f, 0.f, 0.f, 1.f), g_matWVP);
    vProjPos.xyz = vProjPos.xyz / vProjPos.w;
    
    output.vObjectUV.x = (vProjPos.x + 1.f) / 2.f;
    output.vObjectUV.y = 1.f - ((vProjPos.y + 1.f) / 2.f);
        
    return output;
}


float4 PS_Vortex(VS_VortexOut _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    float effectRadius = 0.1f;
    float effectAngle = 1.f * PI;
    
    float2 center = _in.vObjectUV;
    
    float2 uv = (_in.vPosition.xy / g_RenderResolution.xy) - center;
    
    float len = length(uv * float2(g_RenderResolution.x / g_RenderResolution.y, 1.));
    float angle = atan2(uv.y, uv.x) + effectAngle * smoothstep(effectRadius, 0., len);
    float radius = length(uv);

    vColor = g_tex_0.Sample(g_sam_0, float2(radius * cos(angle), radius * sin(angle)) + center);
    
    return vColor;
}





// ==========================
// Blur Shader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : Target
// ===========================
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

static float CrossFilter[13] = { 0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f, 1.f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f };
static float Total = 6.2108f;

struct VS_OUT_BLUR
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float InstID : FOG;
};

VS_OUT_BLUR VS_Blur(VS_IN _in, uint _InstID : SV_InstanceID)
{
    VS_OUT_BLUR output = (VS_OUT_BLUR) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    output.InstID = _InstID;
    
    return output;
}

float4 PS_Blur(VS_OUT_BLUR _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
        
    float2 vUVStep = 1.f / g_RenderResolution;
    vUVStep *= 5.4f;
    
    if (_in.InstID == 0)
    {
        for (int i = 0; i < 13; ++i)
        {
            float2 vUV = _in.vUV + float2(vUVStep.x * (-6 + i), 0.f);
            vColor += g_tex_0.Sample(g_sam_0, vUV) * CrossFilter[i];
        }
    }
    else if (_in.InstID == 1)
    {
        for (int j = 0; j < 13; ++j)
        {
            float2 vUV = _in.vUV + float2(0.f, vUVStep.y * (-6 + j));
            vColor += g_tex_0.Sample(g_sam_0, vUV) * CrossFilter[j];
        }
    }
    vColor /= Total;
    
    return saturate(vColor);
}


VS_OUT VS_EffectMerge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_EffectMerge(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 vEffect = g_tex_0.Sample(g_sam_0, _in.vUV);
    float4 vEffectBlur = g_tex_1.Sample(g_sam_0, _in.vUV);
    
    float4 vBloom = pow(abs(pow(vEffect, 2.2f)) + abs(pow(vEffectBlur, 2.2f)), 1.f / 2.2f);
    
    return vBloom;
}

#endif
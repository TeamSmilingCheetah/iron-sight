#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

// ============================
// DirLightShader
// g_int_0 : Light Index
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// ============================
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

VS_OUT VS_DirLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target;
    float4 vSpecular : SV_Target1;
};

PS_OUT PS_DirLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
        
    // PositionTarget 에서 물체의 위치값을 확인
    float4 vViewPos = g_tex_0.Sample(g_sam_1, _in.vUV);    
    
    // 기록된 물체가 없으면, 광원계산 중단
    if (0.f == vViewPos.a)
    {
        discard;
    }
    
    float3 vViewNormal = g_tex_1.Sample(g_sam_1, _in.vUV).xyz;
                
    float3 vLightColor = (float3) 0.f;
    float3 vSpecPow = 0.f;    
    CalcLight3D(g_int_0, vViewPos.xyz, vViewNormal, vLightColor, vSpecPow);
    
    output.vDiffuse = float4(vLightColor, 1.f);
    output.vSpecular = float4(vSpecPow, 1.f);
        
    return output;
}




// ============================
// PointLightShader
// mesh    : SphereMesh
// g_int_0 : Light Index
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// g_mat_0 : Inverse matrix
// ============================
VS_OUT VS_PointLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // Pixel 좌표
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;    
        
    // PositionTarget 에서 물체의 위치값을 확인
    float4 vViewPos = g_tex_0.Sample(g_sam_1, vScreenUV);
    
    // 기록된 물체가 없으면, 광원계산 중단
    if (0.f == vViewPos.a)
    {
        discard;
    }
    
    // View 역행렬을 곱해서 기록된 Position 의 월드좌표를 알아낸다.
    // VolumeMesh 의 월드 역행렬을 곱해서 VolumeMesh 모델링(로컬) 공간으로 이동한다.
    float3 vLocalPos = mul(float4(vViewPos.xyz, 1.f), g_mat_0).xyz;
    
    // VolumeMesh 외부면 광원계산 중단
    if (length(vLocalPos) > 0.5f)
    {
        discard;
    }   
   
    float3 vViewNormal = g_tex_1.Sample(g_sam_1, vScreenUV).xyz;
                
    float3 vLightColor = (float3) 0.f;
    float3 vSpecPow = 0.f;
    CalcLight3D(g_int_0, vViewPos.xyz, vViewNormal, vLightColor, vSpecPow);
    
    output.vDiffuse = float4(vLightColor, 1.f);
    output.vSpecular = float4(vSpecPow, 1.f);
        
    return output;
}









#endif
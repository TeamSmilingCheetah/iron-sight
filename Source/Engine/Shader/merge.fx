#ifndef _MERGE
#define _MERGE

#include "value.fx"
#include "func.fx"

Texture2D LUTTex : register(t26);
Texture2D DirectLightingTex : register(t27);

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
VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    // Phong
    if (g_int_0 == 0)
    {
        float3 vColor       = g_tex_0.Sample(g_sam_0, _in.vUV).rgb;
        float4 vViewPos     = g_tex_1.Sample(g_sam_0, _in.vUV);
        float3 vDiffuse     = g_tex_2.Sample(g_sam_0, _in.vUV).rgb;
        float3 vSpecular    = g_tex_3.Sample(g_sam_0, _in.vUV).rgb;
        float3 vEmissive    = g_tex_4.Sample(g_sam_0, _in.vUV).rgb;
                
        if (vViewPos.a == 0.f)
            discard;

        return float4(vColor * (vDiffuse + vEmissive) + vSpecular, 1.f);
    }

    // PBR
    else if (g_int_0 == 1)
	{
		float3 vColor       = g_tex_0.Sample(g_sam_0, _in.vUV).rgb;
		float4 vViewPos     = g_tex_1.Sample(g_sam_0, _in.vUV);
		float3 vViewNormal  = g_tex_2.Sample(g_sam_0, _in.vUV).xyz;
		float  Metallic     = g_tex_3.Sample(g_sam_0, _in.vUV).x;
		float  Roughness    = g_tex_4.Sample(g_sam_0, _in.vUV).x;
		float  AO           = g_tex_5.Sample(g_sam_0, _in.vUV).x;

		if (vViewPos.a == 0.f)
			discard;

        // Environment Lighting
		Matrix matViewInv = g_mat_0;
		float3 vWorldNormal = mul(float4(vViewNormal, 0.f), matViewInv);
		float3 vPixelToEye = normalize(-vViewPos.xyz);
		float3 vViewReflect = reflect(vPixelToEye, vViewNormal);
		float3 vWorldReflect = mul(float4(vViewReflect, 0.f), matViewInv);

        // DiffuseBRDF
		const float3 Fdielectric = (float3) 0.04f;
		float3 F0 = lerp(Fdielectric, vColor, Metallic);
		float3 F = SchlickFresnel(F0, max(0.0, dot(vViewNormal, vPixelToEye)));
		float3 kd = (1.f - F) * (1.f - Metallic);

		float3 diffuseIrradiance = g_texcube_0.Sample(g_sam_0, vWorldNormal).rgb;

		float3 diffuseBRDF = kd * vColor * diffuseIrradiance / PI;

        // SpecularBRDF 
		const uint mipLevels = 5;
		float3 specularIrradiance = g_texcube_1.SampleLevel(g_sam_0, vWorldReflect, Roughness * mipLevels).rgb;

        // LUT는 Clamp Sampler (g_sam_2) 사용
		float2 LUT = LUTTex.Sample(g_sam_2, float2(dot(vPixelToEye, vViewNormal), Roughness)).rg;

		float3 specularBRDF = (LUT.r * F0 + LUT.g) * specularIrradiance;
        
		float3 vDirectLighting = DirectLightingTex.Sample(g_sam_0, _in.vUV).rgb;

        // 환경광에 Ambient Occlusion 적용
		float3 vEnvironmentLighting = (diffuseBRDF + specularBRDF) * AO;

		float4 Output = float4(vDirectLighting + vEnvironmentLighting, 1.f);
        
		return Output;
	}

    // Copy
    else
    {
        float4 vTarget = g_tex_0.Sample(g_sam_0, _in.vUV);
        return vTarget;
    }
}

#endif

#ifndef _MERGE
#define _MERGE

#include "value.fx"
#include "func.fx"

Texture2D LUTTex : register(t26);
Texture2D DirectLightingTex : register(t27);
Texture2D EmissiveTex : register(t28);

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
		float3 Emissive     = EmissiveTex.Sample(g_sam_0, _in.vUV).rgb;

		if (vViewPos.a == 0.f)
			discard;

        // Environment Lighting
		Matrix matViewInv = g_mat_0;
		float3 vWorldNormal = mul(float4(vViewNormal, 0.f), matViewInv);
		float3 vViewPixelToEye = normalize(-vViewPos.xyz);
		float3 vWorldPixelToEye = mul(float4(vViewPixelToEye, 0.f), matViewInv).xyz;
		float3 vWorldReflect = reflect(-vWorldPixelToEye, vWorldNormal);

		float NoV = dot(vWorldNormal, vWorldPixelToEye);

        // DiffuseBRDF
		const float3 Fdielectric = (float3) 0.04;
		float3 F0 = lerp(Fdielectric, vColor, Metallic);
		float3 F = SchlickFresnel(F0, max(0.0, NoV));
		float3 kd = ((float3) 1.0 - F) * (1.0 - Metallic);

		float3 diffuseIrradiance = g_texcube_0.Sample(g_sam_0, vWorldNormal).rgb;

		float3 diffuseIBL = kd * vColor * diffuseIrradiance;    // divide by PI ?

        // SpecularBRDF 
		const uint mipLevels = 5;   // Specular Cubemap의 mipmap 수 .. 너무 높은 레벨까지 사용하면 specular가 잘 드러나지 않음.
		float3 specularIrradiance = g_texcube_1.SampleLevel(g_sam_0, vWorldReflect, Roughness * mipLevels).rgb;

        // LUT는 Clamp Sampler (g_sam_2) 사용
		float2 LUT = LUTTex.Sample(g_sam_2, float2(NoV, Roughness)).rg;

		float3 specularIBL = (LUT.r * F0 + LUT.g) * specularIrradiance;

        // Light 단계에서 계산한 직접광
		float3 vDirectLighting = DirectLightingTex.Sample(g_sam_0, _in.vUV).rgb;

        // 환경광에 Ambient Occlusion 적용
        // TEST(Ssio) : 환경광이 너무 강하게 적용되어서 0.8 곱해 둠
		float3 vEnvironmentLighting = (diffuseIBL + specularIBL) * AO;

		float4 Output = float4(vDirectLighting + vEnvironmentLighting + Emissive, 1.f);
        
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

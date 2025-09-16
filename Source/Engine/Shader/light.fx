#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

// ============================
// DirLightShader
// g_int_0 : Light Index
// g_tex_0 : BaseColorTex
// g_tex_1 : PositionTargetTex
// g_tex_2 : NormalTargetTex
// g_tex_3 : MetallicTex
// g_tex_4 : RoughnessTex
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
    float4 vViewPos = g_tex_1.Sample(g_sam_1, _in.vUV);    
    
    // 기록된 물체가 없으면, 광원계산 중단
    if (0.f == vViewPos.a)
    {
        discard;
    }

	float3 vBaseColor = g_tex_0.Sample(g_sam_1, _in.vUV).rgb;
    float3 vViewNormal = g_tex_2.Sample(g_sam_1, _in.vUV).xyz;
	float Metallic = g_tex_3.Sample(g_sam_1, _in.vUV).x;
	float Roughness = g_tex_4.Sample(g_sam_1, _in.vUV).x;

    // Phong
    //float3 vLightColor = (float3) 0.f;
    //float3 vSpecPow = 0.f;    
    //Phong3D(g_int_0, vViewPos.xyz, vViewNormal, vLightColor, vSpecPow);
    //
    //output.vDiffuse = float4(vLightColor, 1.f);
    //output.vSpecular = float4(vSpecPow, 1.f);

    // PBR
	tLight3DInfo Light = g_Light3DInfo[g_int_0];

    // ViewSpace에서 계산
	float3 pixelToEye = normalize(-vViewPos.xyz); // viewpos에서 원점으로
	float3 vViewLight = normalize(mul(float4(-Light.vDir, 0.f), g_matView).xyz);
	float3 vViewHalfway = normalize(vViewLight + pixelToEye);

	//normalize(vViewNormal);

	float NoL = max(0.f, dot(vViewNormal, vViewLight));
	float NoH = max(0.f, dot(vViewNormal, vViewHalfway));
	float NoV = max(0.f, dot(vViewNormal, pixelToEye));
	float LoH = max(0.f, dot(vViewLight, vViewHalfway));

    // Diffuse Term
	const float3 Fdielectric = (float3) 0.04;
	float3 F0 = lerp(Fdielectric, vBaseColor, Metallic);
	float3 F = SchlickFresnel(F0, LoH);
	float3 kd = ((float3) 1.f - F) * (1.f - Metallic);
	float3 diffuseBRDF = kd * vBaseColor;   // divide by PI ? 

    // Specular Term
	float D = NDF_GGX(NoH, Roughness);
	float G = Smith_SchlickGGX(NoL, NoV, Roughness);

	float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NoL * NoV);

    // 빛 세기
	float3 Li = Light.info.vColor * NoL;
    
    float3 finalColor = (diffuseBRDF + specularBRDF) * Li;

    // TEST(Ssio): Diffuse Texture에 DirectLighting 누적
    // TODO(Ssio) : LightMRT -> PBR 고려하여 RT 하나로 줄이기 고려
	output.vDiffuse.rgb += finalColor;
    
    return output;
}


// ============================
// PointLightShader
// mesh    : SphereMesh
// g_int_0 : Light Index
// g_tex_0 : BaseColorTex
// g_tex_1 : PositionTargetTex
// g_tex_2 : NormalTargetTex
// g_tex_3 : MetallicTex
// g_tex_4 : RoughnessTex
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
    float4 vViewPos = g_tex_1.Sample(g_sam_1, vScreenUV);
    
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

	float3 vBaseColor = g_tex_0.Sample(g_sam_1, vScreenUV).rgb;
    float3 vViewNormal = g_tex_2.Sample(g_sam_1, vScreenUV).xyz;
	float Metallic = g_tex_3.Sample(g_sam_1, vScreenUV).x;
	float Roughness = g_tex_4.Sample(g_sam_1, vScreenUV).x;

    // Phong
    //float3 vLightColor = (float3) 0.f;
    //float3 vSpecPow = 0.f;
	//Phong3D(g_int_0, vViewPos.xyz, vViewNormal, vLightColor, vSpecPow);
    //
    //output.vDiffuse = float4(vLightColor, 1.f);
    //output.vSpecular = float4(vSpecPow, 1.f);
    
    // PBR
	tLight3DInfo Light = g_Light3DInfo[g_int_0];

    // ViewSpace에서 계산
	float3 pixelToEye = normalize(-vViewPos.xyz); // viewpos에서 원점으로
	
	// Point Light: 광원 위치에서 픽셀까지의 방향 계산
	float3 vLightViewPos = (mul(float4(Light.WorldPos, 1.f), g_matView)).xyz;
	float3 vViewLight = normalize(vLightViewPos - vViewPos.xyz);
	float3 vViewHalfway = normalize(vViewLight + pixelToEye);
    
	float NoL = max(0.f, dot(vViewNormal, vViewLight));
	float NoH = max(0.f, dot(vViewNormal, vViewHalfway));
	float NoV = max(0.f, dot(vViewNormal, pixelToEye));
	float LoH = max(0.f, dot(vViewLight, vViewHalfway));

    // Diffuse Term
	const float3 Fdielectric = (float3) 0.04;
	float3 F0 = lerp(Fdielectric, vBaseColor, Metallic);
	float3 F = SchlickFresnel(F0, LoH);
	float3 kd = ((float3) 1.f - F) * (1.f - Metallic);
	float3 diffuseBRDF = kd * vBaseColor; // divide with PI ? 

    // Distance attenuation
	float fDist = length(vLightViewPos - vViewPos.xyz);
	float fDistRatio = saturate(1.f - (fDist / Light.Radius));

    // Specular Term
	float D = NDF_GGX(NoH, Roughness);
	float3 G = Smith_SchlickGGX(NoL, NoV, Roughness);

	float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NoL * NoV);

    // 빛 세기
	float3 Li = Light.info.vColor * NoL * fDistRatio;
    
    float3 finalColor = (diffuseBRDF + specularBRDF) * Li;
    
	// TEST(Ssio): Diffuse Texture에 DirectLighting 누적
    // TODO(Ssio) : LightMRT -> PBR 고려하여 RT 하나로 줄이기 고려
	output.vDiffuse += float4(finalColor, 1.f);
    
    return output;
}

#endif

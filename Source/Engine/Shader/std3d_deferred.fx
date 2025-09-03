#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "value.fx"
#include "func.fx"

// g_int_3 : ObjectID
// g_int_2 : ParentID

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

    int parentID : TEXCOORD1;
    int objectID : TEXCOORD2;
};

VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV.xy;

    // LocalPos 를 ViewPos 로 변환
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;

    // LocalSpace Normal(법선)방향 정보를 월드 기준으로 변경
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBirnormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);

    // ID 전달
    output.parentID = g_int_2;
    output.objectID = g_int_3;

    return output;
}

struct VS_IN_Inst
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
		
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
    
    // Per Instance Data    
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint iRowIndex : ROWINDEX; // 자신의 애니메이션 최종 행렬 데이터가 몇번째 행에 있는지
    int parentID : TEXCOORD1;
    int objectID : TEXCOORD2;
};


VS_OUT VS_Std3D_Deferred_Inst(VS_IN_Inst _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, _in.iRowIndex);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vUV = _in.vUV;
    
    // LocalPos 를 ViewPos 로 변환
    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV).xyz;
    
    // LocalSpace Normal(법선)방향 정보를 월드 기준으로 변경
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV).xyz);
    output.vViewBirnormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV).xyz);

    // 인스턴싱 데이터에서 ID 전달
    output.parentID = _in.parentID;
    output.objectID = _in.objectID;
    
    return output;
}

struct PS_OUT
{
    float4 Color        : SV_Target;
    float4 Normal       : SV_Target1;
    float4 Position     : SV_Target2;
	float4 Metallic     : SV_Target3;
	float4 Roughness    : SV_Target4;
    float4 Emissive     : SV_Target5;
    float4 Data         : SV_Target6;
};

// g_tex_0 : Color Tex
// g_tex_1 : Normal Map
// g_tex_2 : Metallic Tex
// g_tex_3 : Roughness Tex
// g_float_0 : Metallic
// g_float_1 : Roughness

PS_OUT PS_Std3D_Deferred(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;

    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    float3 vNormal = _in.vViewNormal;
	float Metallic = g_float_0;
	float Roughness = g_float_1;
    
	float2 dx = ddx(_in.vUV);
	float2 dy = ddy(_in.vUV);

    // Base Color
    if (g_btex_0)
    {
		vColor = g_tex_0.SampleGrad(g_sam_0, _in.vUV, dx, dy);
	}

    // Normal Map
    if (g_btex_1)
    {
		vNormal = g_tex_1.SampleGrad(g_sam_0, _in.vUV, dx, dy).xyz;

        // 추출한 색상값(0~1 범위) 을 방향벡터 값의 범위(-1 ~ 1) 로 변경한다.
        vNormal = (vNormal * 2.f) - 1.f;

        // tangent space(텍스쳐 공간) 에 있는 각 축이,
        // 적용시킬 표면의 각 방향이 될 수 있도록 하는 회전행렬
        float3x3 matRot =
        {
            _in.vViewTangent,
            _in.vViewBirnormal,
            _in.vViewNormal,
        };

        vNormal = normalize(mul(vNormal, matRot));
    }

    // Metallic Tex
	if (g_btex_2)
	{
		Metallic = g_tex_2.Sample(g_sam_0, _in.vUV).x;
	}

    // Roughness Tex
	if (g_btex_3)
	{
		Roughness = g_tex_3.Sample(g_sam_0, _in.vUV).x;
	}

	output.Color = vColor;
    output.Normal = float4(vNormal, 1.f);
    output.Position = float4(_in.vViewPos, 1.f);
	output.Metallic = Metallic;
	output.Roughness = Roughness;
    output.Emissive = (float4) 0.f;
    output.Data = float4((float) _in.parentID, (float) _in.objectID, 0.f, 0.f);

    return output;
}

#endif

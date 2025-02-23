#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"



// ======================
// LandScape 쉐이더
// MRT    : Deferred
// Domain : Deferred
// Mesh   : LandScapeMesh

// Parameter
#define FACE_X              g_int_0
#define FACE_Z              g_int_1
#define MODE                g_int_2

#define IsHeightMap         g_btex_0
#define HeightMap           g_tex_0

#define IsShowBrush         g_btex_1 && g_float_0
#define BRUSH_TEX           g_tex_1
#define BrushScale          g_vec2_0
#define BrushPos            g_vec2_1

#define COLOR_TEX           g_texarr_0
#define NORMAL_TEX          g_texarr_1
#define HasColorTex         g_btexarr_0
#define HasNormalTex        g_btexarr_1
#define TEXTURE_ARRSIZE     g_int_3

StructuredBuffer<tWeight8>  WEIGHT_MAP : register(t20);
#define WEIGHT_RESOLUTION   g_vec2_2
// ======================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vLocalPos = _in.vPos;
    output.vUV = _in.vUV;
    
    output.vNormal = _in.vNormal;
    output.vTangent = _in.vTangent;
    output.vBinormal = _in.vBinormal;
    
    return output;
}


// HullShader
// 패치단위 입력, 패치를 어떻게 분할할 것인지를 정하는 단계
// HullShader 는 함수가 2개로 구성됨
// 1. HullShader 메인 함수 - 정점마다 호출 됨
// 2. 패치상수함수 (PatchConstantFuncion) - 패치당 1번 호출
struct PatchTess
{
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> _Input, uint _PatchIdx : SV_PrimitiveID)
{
    PatchTess output = (PatchTess) 0.f;
    
    float Level = 8;
   
    
    output.Edge[0] = Level;
    output.Edge[1] = Level;
    output.Edge[2] = Level;
    output.Inside = Level;
    
    return output;
}

// HullShader Main
struct HS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

[domain("tri")] // 패치의 구성단위가 삼각형
[partitioning("integer")] // 정수단위로 분할
//[partitioning("fractional_odd")] // 실수단위로 분할
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64.0)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _Input
               , uint _VtxIdx : SV_OutputControlPointID
               , uint _PatchIdx : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vLocalPos    = _Input[_VtxIdx].vLocalPos;
    output.vUV          = _Input[_VtxIdx].vUV;    
    output.vNormal      = _Input[_VtxIdx].vNormal;
    output.vTangent     = _Input[_VtxIdx].vTangent;
    output.vBinormal    = _Input[_VtxIdx].vBinormal;
    
    return output;

}

// DomainShader
// HullShader -> Tessellator 를 거쳐서 생성된 모든 정점들에 대해서 호출되는 쉐이더
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};


[domain("tri")]
DS_OUT DS_LandScape( PatchTess _tessfactor
                   , float3 _Weight : SV_DomainLocation
                   , const OutputPatch<HS_OUT, 3> _Output)
{
    DS_OUT output = (DS_OUT) 0.f;
     
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;    
    float3 vTangent = (float3) 0.f;
    float3 vNormal = (float3) 0.f;
    float3 vBinormal = (float3) 0.f;
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos   += _Output[i].vLocalPos * _Weight[i];
        vUV         += _Output[i].vUV       * _Weight[i];
        vTangent    += _Output[i].vTangent  * _Weight[i];
        vNormal     += _Output[i].vNormal   * _Weight[i];
        vBinormal   += _Output[i].vBinormal * _Weight[i];
    }    
    
    if (IsHeightMap)
    {
        // 높이맵에서 높이값 가져오기
        float2 vFullUV = vUV / float2(FACE_X, FACE_Z);
       
        // LOD(Level Of Detail) : MipMap Level 
        // Height : 0 ~ 1.f
        float Height = HeightMap.SampleLevel(g_sam_0, vFullUV, 0).r;
        vLocalPos.y = Height;
                
        // 높이를 적용함에 따라, Normal 벡터를 재계산해주어야 한다.
        float LocalStep = 1.f / _tessfactor.Inside;
        float2 vUVStep = LocalStep / float2(FACE_X, FACE_Z);
                
        float LeftHeight = HeightMap.SampleLevel(g_sam_0, vFullUV - float2(vUVStep.x, 0.f), 0).r;
        float RightHeight = HeightMap.SampleLevel(g_sam_0, vFullUV + float2(vUVStep.x, 0.f), 0).r;
        float UpHeight = HeightMap.SampleLevel(g_sam_0, vFullUV - float2(0.f, vUVStep.y), 0).r;
        float DownHeight = HeightMap.SampleLevel(g_sam_0, vFullUV + float2(0.f, vUVStep.y), 0).r;        
        
        float3 vLeft    = float3(vLocalPos.x - LocalStep, LeftHeight    , vLocalPos.z);
        float3 vRight   = float3(vLocalPos.x + LocalStep, RightHeight   , vLocalPos.z);
        float3 vUp      = float3(vLocalPos.x            , UpHeight      , vLocalPos.z + LocalStep);
        float3 vDown    = float3(vLocalPos.x            , DownHeight    , vLocalPos.z - LocalStep);
                
        vLeft  = mul(float4(vLeft , 1.f), g_matWorld).xyz;
        vRight = mul(float4(vRight, 1.f), g_matWorld).xyz;
        vUp    = mul(float4(vUp   , 1.f), g_matWorld).xyz;
        vDown  = mul(float4(vDown , 1.f), g_matWorld).xyz;        
        
        vTangent  = normalize(vRight - vLeft);
        vBinormal = normalize(vDown - vUp);
        vNormal   = normalize(cross(vTangent, vBinormal));
    }
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV);
    output.vUV = vUV;
    
    if (IsHeightMap)
    {
        output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matView).xyz);
        output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matView).xyz);
        output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matView).xyz);
    }
    else
    {
        output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV).xyz);
        output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV).xyz);
        output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV).xyz);
    }
        
    return output;
}

struct PS_OUT
{
    float4 Color    : SV_Target;
    float4 Normal   : SV_Target1;
    float4 Position : SV_Target2;
    float4 Emissive : SV_Target3;
    float4 Data     : SV_Target4;
};

PS_OUT PS_LandScape(DS_OUT _in)
{    
    PS_OUT output = (PS_OUT) 0.f;
        
    float4 vBrush = (float4) 0.f;
    
    if (IsShowBrush && MODE)
    {
        // Brush LeftTop 좌표
        float2 BrushLT = BrushPos - (BrushScale * 0.5f);
        
        // 지형 기준, 픽셀의 위치 구하기
        float2 vBrusUV = _in.vUV / float2(FACE_X, FACE_Z);
        vBrusUV = (vBrusUV - BrushLT) / BrushScale;
        
        if (0.f <= vBrusUV.x && vBrusUV.x <= 1.f
            && 0.f <= vBrusUV.y && vBrusUV.y <= 1.f)
        {
            float BrushAlpha = BRUSH_TEX.Sample(g_sam_0, vBrusUV).a;
            float3 BrushColor = float3(0.8f, 0.8f, 0.f);
            
            vBrush.rgb = (vBrush.rgb * (1 - BrushAlpha)) + (BrushColor * BrushAlpha);
        }
    }
       
    
    float4 vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    float3 vViewNormal = _in.vViewNormal;
    
    if (HasColorTex)
    {        
        float2 vFullUV = _in.vUV / float2(FACE_X, FACE_Z);
        int2 vColRow = vFullUV * WEIGHT_RESOLUTION;
        int WeightMapIdx = WEIGHT_RESOLUTION.x * vColRow.y + vColRow.x;

        vColor = (float4) 0.f;
        
        int MaxIdx = -1;
        float WeightMax = 0.f;
                
        // 편미분
        float2 derivX = ddx(_in.vUV);
        float2 derivY = ddy(_in.vUV);
        
        for (int i = 0; i < TEXTURE_ARRSIZE; ++i)
        {
            float Weight = WEIGHT_MAP[WeightMapIdx].arrWeight[i];
            
            if (0.f != Weight)
            {
                //vColor += COLOR_TEX.SampleLevel(g_sam_0, float3(_in.vUV, i), 4) * Weight;
                vColor += COLOR_TEX.SampleGrad(g_sam_0, float3(_in.vUV, i), derivX * 0.3f, derivY * 0.3f) * Weight;
            }
                        
            // 제일 높았던 가중치를 기록
            if (WeightMax < Weight)
            {
                WeightMax = Weight;
                MaxIdx = i;
            }
        }
        
        if (MaxIdx != -1)
        {
            //float3 vNormal = NORMAL_TEX.SampleLevel(g_sam_0, float3(_in.vUV, MaxIdx), 4);
            float3 vNormal = NORMAL_TEX.SampleGrad(g_sam_0, float3(_in.vUV, MaxIdx), derivX * 0.3f, derivY * 0.3f);
            vNormal = vNormal * 2.f - 1.f;
        
            float3x3 Rot =
            {
                _in.vViewTangent,
                _in.vViewBinormal,
                _in.vViewNormal
            };
        
            vViewNormal = normalize(mul(vNormal, Rot));
        }
    }
    
    output.Color    = float4(vColor.xyz + vBrush.rgb * 0.5f, 1.f);
    output.Emissive = float4(vBrush.rgb, 1.f);
    output.Normal   = float4(vViewNormal, 1.f);
    output.Position = float4(_in.vViewPos, 1.f);
    
    return output;
}



#endif
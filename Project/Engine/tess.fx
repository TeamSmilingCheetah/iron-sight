#ifndef _TESS
#define _TESS

#include "value.fx"

// ================
// 테셀레이션 쉐이더
// MRT    : Swapchain
// Domain : Forward
// Mesh   : RectMesh
// ================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Tess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vLocalPos = _in.vPos;
    output.vUV = _in.vUV;
    
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
    
    float Level = 1;    
    if(g_float_0)
        Level = g_float_0;
    
    output.Edge[0] = Level;
    output.Edge[1] = Level;
    output.Edge[2] = Level;
    output.Inside  = Level;
    
    return output;
}

// HullShader Main
struct HS_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

[domain("tri")]           // 패치의 구성단위가 삼각형
//[partitioning("integer")] // 정수단위로 분할
[partitioning("fractional_odd")] // 실수단위로 분할
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(32.0)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_Tess(  InputPatch<VS_OUT, 3> _Input
               , uint _VtxIdx : SV_OutputControlPointID
               , uint _PatchIdx : SV_PrimitiveID )
{    
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vLocalPos = _Input[_VtxIdx].vLocalPos;
    output.vUV = _Input[_VtxIdx].vUV;
    
    return output;

}

// DomainShader
// HullShader -> Tessellator 를 거쳐서 생성된 모든 정점들에 대해서 호출되는 쉐이더
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


[domain("tri")]
DS_OUT DS_Tess( PatchTess _tessfactor
             ,  float3 _Weight : SV_DomainLocation
             ,  const OutputPatch<HS_OUT, 3> _Output)
{
    DS_OUT output = (DS_OUT) 0.f;
     
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Output[i].vLocalPos * _Weight[i];
        vUV += _Output[i].vUV * _Weight[i];
    }    
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
        
    return output;
}

float4 PS_Tess(DS_OUT _in) : SV_Target
{    
    return float4(1.f, 0.f, 1.f, 1.f);
}


#endif
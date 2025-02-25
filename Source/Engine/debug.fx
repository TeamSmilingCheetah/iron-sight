#ifndef _DEBUG
#define _DEBUG

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV  : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_DebugShape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    return g_vec4_0;
}


struct VS_SPHERE_IN
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
};

struct VS_SPHERE_OUT
{
    float4 vPosition : SV_Position;
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
};


VS_SPHERE_OUT VS_DebugShapeSphere(VS_SPHERE_IN _in)
{
    VS_SPHERE_OUT output = (VS_SPHERE_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_DebugShapeSphere(VS_SPHERE_OUT _in) : SV_Target
{
    float3 vEye = normalize(_in.vViewPos);

    if (0.f < dot(_in.vViewNormal, vEye))
    {
        float Alpha = pow(1.f - saturate(dot(vEye, _in.vViewNormal)), 2.f);
        return float4(g_vec4_0.rgb, Alpha);
    }
    else
    {
        float Alpha = pow(saturate(dot(vEye, _in.vViewNormal)), 2.f);
        return float4(g_vec4_0.rgb, Alpha);
    }
}


struct VS_LINE_IN
{
    float3 vPos : POSITION;
};

struct VS_LINE_OUT
{
    float3 vPos : POSITION;
};

struct GS_LINE_OUT
{
    float4 vPosition : SV_Position;
};

VS_LINE_OUT VS_DebugShapeLine(VS_IN _in)
{
    VS_LINE_OUT output = (VS_LINE_OUT) 0.f;

    output.vPos = _in.vPos;

    return output;
}

[maxvertexcount(32)]
void GS_DebugShapeLine(point VS_LINE_OUT _in[1]
                            , inout LineStream<GS_LINE_OUT> _OutStream)
{
    GS_LINE_OUT Start = (GS_LINE_OUT) 0.f;
    GS_LINE_OUT End = (GS_LINE_OUT) 0.f;

    Start.vPosition = g_vec4_1;
    End.vPosition = g_vec4_2;

    Start.vPosition = mul(mul(float4(Start.vPosition.xyz, 1.f), g_matView), g_matProj);
    End.vPosition = mul(mul(float4(End.vPosition.xyz, 1.f), g_matView), g_matProj);

    _OutStream.Append(Start);
    _OutStream.Append(End);
    _OutStream.RestartStrip();
}

float4 PS_DebugShapeLine(GS_LINE_OUT _in) : SV_Target
{
    return g_vec4_0;
}

#endif

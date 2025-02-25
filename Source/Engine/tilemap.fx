#ifndef _TILEMAP
#define _TILEMAP

#include "value.fx"
#include "func.fx"

#define TileAtlas   g_tex_0
#define COL         g_int_0
#define ROW         g_int_1

struct tTileInfo
{
    float2 TileLT;
    float2 TileSlice;
};

StructuredBuffer<tTileInfo> g_Buffer : register(t16);

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_TileMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV * float2(COL, ROW);

    return output;
}


float4 PS_TileMap(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    if(g_btex_0)
    {
        int2 ColRow = floor(_in.vUV);
        int idx = ColRow.y * COL + ColRow.x;

        float2 vAtlasUV = g_Buffer[idx].TileLT + (frac(_in.vUV) * g_Buffer[idx].TileSlice);
        vOutColor = TileAtlas.Sample(g_sam_1, vAtlasUV);
    }
    else
    {
        vOutColor = GetDebugColor(_in.vUV, 10);
    }

    float3 LightColor = float3(0.f, 0.f, 0.f);

    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }

    vOutColor.rgb *= LightColor;

    return vOutColor;
}

#endif

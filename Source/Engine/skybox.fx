#ifndef _SKYBOX
#define _SKYBOX
#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float4 vUV : TEXCOORD;  
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vCubeUV : TEXCOORD1;
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(mul(float4(_in.vPos * 2.f, 0.f), g_matView), g_matProj);
    output.vPosition.z = output.vPosition.w;    
    output.vUV = _in.vUV;
    output.vCubeUV = normalize(_in.vPos);
    
    
    return output;
}

float4 PS_SkyBox(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
      
    // Sphere
    if(g_int_0 == 0)
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    }
    
    // Cube
    else if(g_int_0 == 1)
    {
        if (g_btexcube_0)
        {
            vColor = g_texcube_0.Sample(g_sam_0, _in.vCubeUV);
        }
    }
    
    return vColor;
}



#endif
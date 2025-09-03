#ifndef _MERGE
#define _MERGE

#include "value.fx"


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
        float4 vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        float4 vViewPos = g_tex_1.Sample(g_sam_1, _in.vUV);
        float4 vDiffuse = g_tex_2.Sample(g_sam_1, _in.vUV);
        float4 vSpecular = g_tex_3.Sample(g_sam_1, _in.vUV);
        float4 vEmissive = g_tex_4.Sample(g_sam_1, _in.vUV);
                
        if (vViewPos.a == 0.f)
            discard;

        return float4(vColor.rgb * (vDiffuse.rgb + vEmissive.rgb) + vSpecular.rgb, 1.f);
    }

    // PBR
    else if (g_int_0 == 1)
	{
		float4 vViewPos = g_tex_1.Sample(g_sam_1, _in.vUV);
		float4 vDiffuse = g_tex_2.Sample(g_sam_1, _in.vUV);
		//float4 vSpecular = g_tex_3.Sample(g_sam_1, _in.vUV);

		if (vViewPos.a == 0.f)
			discard;

		//return vDiffuse + vSpecular;
		return vDiffuse;
	}

    // Copy
    else
    {
        float4 vTarget = g_tex_0.Sample(g_sam_1, _in.vUV);
        return vTarget;
    }
}

#endif

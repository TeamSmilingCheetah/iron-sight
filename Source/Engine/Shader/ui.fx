#ifndef _UI
#define _UI

#include "value.fx"

// Settings
// Forward Rendering
// Render Target : 0 (color)
// Domain : UI (가장 위)

// values
#define Color g_vec4_0
#define UseImage g_btex_0
#define Image g_tex_0

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;
};

struct VS_IN_Inst
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;
	
	// Per Instance Data    
	row_major matrix matWorld : WORLD;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
};


struct VS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;  
};

VS_OUT VS_UI(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
	output.vUV = _in.vUV;

	return output;
}


VS_OUT VS_UI_Inst(VS_IN_Inst _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
	output.vUV = _in.vUV;

	return output;
}


float4 PS_UI(VS_OUT _in) : SV_Target
{
	float4 output = (float4) 0.f;

	if (UseImage)
	{
		output = Image.Sample(g_sam_0, _in.vUV);
	}
	else
	{
		output = Color; // 배경
	}

	return output;
}



#endif

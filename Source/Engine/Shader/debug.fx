#ifndef _DEBUG
#define _DEBUG

#include "value.fx"

// =======
// General
// =======
#define Color g_vec4_0;


struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_DebugShape(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

	return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
	return Color;
}

// =======
// Sphere
// =======

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

// ====
// Line
// ====

struct GS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_DebugShapeLine(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = float4(_in.vPos, 1.f);

	return output;
}

[maxvertexcount(32)]
void GS_DebugShapeLine(point VS_OUT _in[1]
							, inout LineStream<GS_OUT> _OutStream)
{
	GS_OUT Start = (GS_OUT) 0.f;
	GS_OUT End = (GS_OUT) 0.f;

	Start.vPosition = g_vec4_1;
	End.vPosition = g_vec4_2;

	Start.vPosition = mul(mul(float4(Start.vPosition.xyz, 1.f), g_matView), g_matProj);
	End.vPosition = mul(mul(float4(End.vPosition.xyz, 1.f), g_matView), g_matProj);

	_OutStream.Append(Start);
	_OutStream.Append(End);
	_OutStream.RestartStrip();
}

float4 PS_DebugShapeLine(GS_OUT _in) : SV_Target
{
	return Color;
}

// ========
// Skeleton
// ========

VS_OUT VS_DebugSkeleton(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = float4(_in.vPos, 1.f);

	return output;
}

StructuredBuffer<int> arrBoneParent : register(t19);

[maxvertexcount(256)]
void GS_DebugSkeleton(point VS_OUT _in[1]
							, inout LineStream<GS_OUT> _OutStream)
{
	GS_OUT Start = (GS_OUT) 0.f;
	GS_OUT End = (GS_OUT) 0.f;

	int count = 0;
	
	for (int i = 1; i < g_iBoneCount; ++i)
	{
		if (count == 128)
			break;
		
		float4 vStartLocalPos = mul(float4(0.f, 0.f, 0.f, 1.f), g_arrPureBoneMat[i]);
		float4 vEndLocalPos = mul(float4(0.f, 0.f, 0.f, 1.f), g_arrPureBoneMat[arrBoneParent[i]]);

		if ((vStartLocalPos.x == 0.f && vStartLocalPos.y == 0.f && vStartLocalPos.z == 0.f)
		|| (vEndLocalPos.x == 0.f && vEndLocalPos.y == 0.f && vEndLocalPos.z == 0.f))
			continue;
		
		// i와 arrBoneParent[i]를 연결한다.
		Start.vPosition = mul(vStartLocalPos, g_matWVP);
		End.vPosition = mul(vEndLocalPos, g_matWVP);
		
		_OutStream.Append(Start);
		_OutStream.Append(End);
		_OutStream.RestartStrip();

		++count;
	}
}

float4 PS_DebugSkeleton(GS_OUT _in) : SV_Target
{
	return Color;
}



#endif

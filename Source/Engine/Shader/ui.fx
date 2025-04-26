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

VS_OUT VS_UI_Cardinal(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

	float rotY = g_float_0;
	rotY /= 360.f; // 0 ~ 1로 매핑
		
	const float offset = 0.02114f;
		
	output.vUV = _in.vUV + float2(rotY + offset - 0.5f, 0.f);

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
	
	// 이미지가 있다면 샘플링
	if (UseImage)
	{
		output = Image.Sample(g_sam_0, _in.vUV);
	}

	// 이미지가 없거나 a 값이 0인 부분은 배경색으로 채움
	if (output.a == 0.f)
	{
		output = Color;
	}

	return output;
}

float4 PS_UI_HP(VS_OUT _in) : SV_Target
{
	// g_float_0 : aspect
	// g_float_1 : semiHP
	// g_float_2 : curHP
	
	const float curHP = g_float_2;	// 최대 체력에 대한 비율
	const float semiHP = g_float_1;
	
	// cur hp
	if (_in.vUV.x < curHP)
	{
		const float offset = 0.4f;	// 체력바가 빨개지기 시작하는 시점
		
		// 체력 비율에 따라 색상 결정
		float ratio = saturate(curHP / (semiHP - offset));
		
		return float4(1.f, ratio, ratio, Color.a * 2.f);
	}
	
	// semi max hp
	else if (_in.vUV.x < semiHP)
	{
		const float dx = 0.02f;		// stripe pattern 너비
		const float speed = 1.5f;	// 움직이는 속도
		float offset = g_Time * speed;		// offset
		offset = (offset - floor(offset)) * 2 * dx;	// 0 ~ 2*dx로 범위 제한
		
		// 몫 계산
		const float aspect = g_float_0; // ui box의 aspect ratio
		float u = _in.vUV.x + _in.vUV.y / aspect + 2 * dx - offset;	// u가 0보다 작을 경우를 대비해 2*dx를 더해줌. (조건문은 cost가 높다고 해서 사용 안함)
		int count = 0;
		while (u > dx)
		{
			u -= dx;
			++count;
		}

		// 홀수면 흰색, 짝수면 color로 채움
		return count % 2 == 1 ? float4(1.f, 1.f, 1.f, Color.a) : Color;
	}

	// max hp
	else
	{
		return Color;	// 배경색
	}
	
}


#endif

#include "pch.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"


CameraEffect::CameraEffect()
	: CScript(SCRIPT_TYPE::CAMERAEFFECT)
	, m_HPLow(false)
{
}

CameraEffect::~CameraEffect()
{
}

void CameraEffect::Begin()
{
}

void CameraEffect::Tick()
{
}

void CameraEffect::HitEffect()
{
	// FadeInfo g_int_0        // 페이드 활성화 (0: 비활성, 1: 페이드 아웃, 2: 페이드 인)
	// HPLow g_int_1           // HP 낮음 신호
	// DamageEffect g_int_2    // 피격 효과 활성화 (0: 비활성, 1: 활성)
	// 
	// FadeStart g_float_0     // 페이드 시작 시간
	// DamageTime g_float_1    // 피격 시간
	// EffectDuration g_float_2  // 효과 지속 시간 (초)
	// 
	// DamageColor g_vec4_0    // 피격 효과 색상 (r, g, b, a)
	// DamageTexture g_tex_0   // 피격 효과 텍스쳐

	// 피격 효과 활성화
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_2, 1);

	// 색상 설정
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_1, Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// 시간 설정
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_1, g_Data.Time);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_2, 1.f);
}

void CameraEffect::HPLow()
{
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);

	pMtrl->SetScalarParam(SCALAR_PARAM::INT_1, 1);
}

void CameraEffect::HPFine()
{
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);

	pMtrl->SetScalarParam(SCALAR_PARAM::INT_1, 0);
}

void CameraEffect::SaveComponent(FILE* PFile)
{
}

void CameraEffect::LoadComponent(FILE* PFile)
{
}

void CameraEffect::LoadComponentReference()
{
}

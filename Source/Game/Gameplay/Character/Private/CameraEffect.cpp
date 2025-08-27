#include "pch.h"
#include "Game/Gameplay/Character/Public/CameraEffect.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"


CameraEffect::CameraEffect()
	: CScript(SCRIPT_TYPE::CAMERAEFFECT)
	, m_HPLow(false)
	, m_FadeOut(false)
	, m_FadeIn(false)
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

	// 페이드 인/아웃 상태 체크
	if (m_FadeIn || m_FadeOut)
	{
		Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);
		void* pStartTime = pMtrl->GetScalarParam(SCALAR_PARAM::FLOAT_0);
		float startTime = *(float*)pStartTime;
		float currentTime = g_Data.Time;
		float elapsedTime = currentTime - startTime;

		// 페이드 효과가 1초 이상 지속되면 종료
		if (elapsedTime >= 1.01f)
		{
			if (m_FadeIn)
			{
				// 페이드 인 완료 (쉐이더 bool 값 초기화를 통해 다른 PostProcess 효과가 제대로 적용 될 수 있게 해준다.
				pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, 0);
				m_FadeIn = false;
			}

			if (m_FadeOut)
			{
				// 페이드 아웃 완료
				m_FadeOut = false;
			}
		}
	}
}

void CameraEffect::FadeIn()
{
	// shader코드 내에서 진행됨.
	// 시간은 1초동안 지속됨.
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);

	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, 2);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, g_Data.Time);
	m_FadeIn = true;
}

void CameraEffect::FadeOut()
{
	// shader코드 내에서 진행됨.
	// 시간은 1초동안 지속됨.
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);

	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, 1);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, g_Data.Time);
	m_FadeOut = true;
}

void CameraEffect::FadeInOutOff()
{
	Ptr<CMaterial> pMtrl = UIRender()->GetMaterial(0);

	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, 0);
	pMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, g_Data.Time);
	m_FadeOut = false;
	m_FadeIn = false;
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

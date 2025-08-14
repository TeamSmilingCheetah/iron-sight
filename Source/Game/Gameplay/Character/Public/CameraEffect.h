#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"


class CameraEffect :
    public CScript
{
private:
	bool	m_HPLow;
	bool	m_FadeOut;
	bool	m_FadeIn;

public:
	void Begin() override;
	void Tick() override;

	void FadeIn();
	void FadeOut();
	void FadeInOutOff();

	void HitEffect();
	void HPLow();
	void HPFine();

	// 페이드 상태 확인 함수들
	bool IsFading() const { return m_FadeIn || m_FadeOut; }
	bool IsFadingIn() const { return m_FadeIn; }
	bool IsFadingOut() const { return m_FadeOut; }

	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;
	void LoadComponentReference() override;

public:
	CLONE(CameraEffect);
	CameraEffect();
	~CameraEffect() override;
};


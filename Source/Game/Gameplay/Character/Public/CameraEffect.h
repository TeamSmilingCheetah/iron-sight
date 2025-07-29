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

	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;
	void LoadComponentReference() override;

public:
	CLONE(CameraEffect);
	CameraEffect();
	~CameraEffect() override;
};


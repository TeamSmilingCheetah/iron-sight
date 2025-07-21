#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"


class CameraEffect :
    public CScript
{
private:
	bool	m_HPLow;

public:
	void Begin() override;
	void Tick() override;

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


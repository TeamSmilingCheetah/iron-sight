#pragma once
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"

class CUIRender
	: public CRenderComponent
{
private:

public:
	void FinalTick() override;
	void Render() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _FILE) override;


public:
	CLONE(CUIRender);
	CUIRender();
	~CUIRender();
};

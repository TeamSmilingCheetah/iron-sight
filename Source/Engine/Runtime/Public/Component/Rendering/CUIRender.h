#pragma once
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"

class CUIRender
	: public FRenderComponent
{
private:

public:
	void Init() override;
	void FinalTick() override;
	void Render() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _FILE) override;


public:
	CLONE(CUIRender);
	CUIRender();
	~CUIRender();
};

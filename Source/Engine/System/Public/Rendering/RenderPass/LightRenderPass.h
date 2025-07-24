#pragma once
#include "RenderPass.h"

class FLightRenderPass
	: public FRenderPass
{
public:
	CLONE(FLightRenderPass);
	FLightRenderPass() = default;
	~FLightRenderPass() override = default;

	void Execute() override;
};

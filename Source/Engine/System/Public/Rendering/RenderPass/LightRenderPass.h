#pragma once
#include "RenderPass.h"

class FLightRenderPass
	: public IRenderPass
{
private:
	CStructuredBuffer* LightInstancingBuffer;

public:
	CLONE(FLightRenderPass);
	FLightRenderPass();
	~FLightRenderPass() override;

	void Execute(FRenderPassParameters& PParams) override;
	void RenderDirectionalLights(const FRenderPassParameters& InParams);
	void RenderPointLights(const FRenderPassParameters& InParams);
};

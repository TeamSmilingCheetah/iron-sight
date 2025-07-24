#pragma once
#include "RenderPass.h"

class FGeometryRenderPass
	: public FRenderPass
{
public:
	CLONE(FGeometryRenderPass);
	FGeometryRenderPass() = default;
	~FGeometryRenderPass() override = default;

	void Execute() override;
};

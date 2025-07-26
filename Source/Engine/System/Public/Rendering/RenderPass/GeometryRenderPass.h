#pragma once
#include "RenderPass.h"

struct FRenderCommand;

class FGeometryRenderPass
	: public IRenderPass
{
public:
	void Execute(FRenderPassParameters& InParams) override;

	FGeometryRenderPass() = default;
	~FGeometryRenderPass() override = default;
	CLONE(FGeometryRenderPass);
};

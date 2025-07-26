#pragma once
#include "RenderPass.h"

class FMergeRenderPass
	: public IRenderPass
{
public:
	void Execute(FRenderPassParameters& InParams) override;
	void ExecuteMinimap() const;

	// Special Member Function
	FMergeRenderPass() = default;
	~FMergeRenderPass() override = default;
	CLONE(FMergeRenderPass);
};

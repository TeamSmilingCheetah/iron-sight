#pragma once
#include "RenderPass.h"

class FMergeRenderPass
	: public FRenderPass
{
public:
	CLONE(FMergeRenderPass);
	FMergeRenderPass() = default;
	~FMergeRenderPass() override = default;

	void Execute() override;
};

#pragma once
#include "AssetUI.h"

class SkeletonUI :
	public AssetUI
{
private:
	int m_SelectedBoneIndex;

public:
	virtual void Render_Update() override;

private:
	void RenderBoneDetails(int boneIndex);
	void RenderBoneTransform(const Matrix& matrix, const char* label);

public:
	SkeletonUI();
	~SkeletonUI() override;
};

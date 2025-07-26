#pragma once
#include "Engine/Core/Public/CEntity.h"

class FMultiRenderTarget :
	public CEntity
{
private:
	int RenderTargetCount;
	Ptr<CTexture> RenderTarget[8];
	Vec4 ClearColor[8];
	D3D11_VIEWPORT Viewport;
	Ptr<CTexture> DepthStencilTexture;

public:
	void Init(const vector<Ptr<CTexture>>& InTextureVector, const Ptr<CTexture>& InDepthStencilTexture);
	void BindTargets() const;
	void SetClearColor(int InIdx, const Vec4& InColor);
	void SetClearColorAll(const Vec4& InColor);
	void ClearAll() const;
	void ClearRenderTargets() const;
	void ClearDepthStencil() const;

	// Getter
	const D3D11_VIEWPORT& GetViewPort() const { return Viewport; }
	Ptr<CTexture> GetTexture(UINT InIndex) const
	{
		if (InIndex >= static_cast<UINT>(RenderTargetCount))
		{
			LOG_ERROR("[Render][MRT] MRT Texture Index Out of Range");
			assert(!"Render Target Texture Index Out of Range");
		}
		return RenderTarget[InIndex];
	}

	// Special Member Function
	FMultiRenderTarget() = default;
	~FMultiRenderTarget() override = default;
	CLONE_DISABLE(FMultiRenderTarget);
};

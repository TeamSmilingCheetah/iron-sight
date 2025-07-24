#include "pch.h"
#include "Engine/System/Public/Rendering/RenderTarget/MultiRenderTarget.h"

/**
 * @brief Output Merger
 */
void FMultiRenderTarget::SetRenderTargets() const
{
	// ViewPort Setting
	CONTEXT->RSSetViewports(1, &MViewport);

	// DX11에서 처리할 수 있는 최대 갯수의 Texture View 생성
	vector<ID3D11RenderTargetView*> RenderTargetView(8, nullptr);

	// 준비해 둔 Texture 삽입
	for (int i = 0; i < MRenderTargetCount; ++i)
	{
		RenderTargetView[i] = MTargetTexture[i]->GetRTV().Get();
	}

	// Texture View와 준비한 Texture 갯수, 존재한다면 Depth Stencil Texture까지 Output Merger한테 전달
	if (MDepthStencilTexture.Get())
	{
		CONTEXT->OMSetRenderTargets(MRenderTargetCount, RenderTargetView.data(), MDepthStencilTexture->GetDSV().Get());
	}
	else
	{
		CONTEXT->OMSetRenderTargets(MRenderTargetCount, RenderTargetView.data(), nullptr);
	}
}

void FMultiRenderTarget::Init(const vector<Ptr<CTexture>>& PTextureVector, const Ptr<CTexture>& PDepthStencilTexture)
{
	MRenderTargetCount = PTextureVector.size();

	for (int i = 0; i < MRenderTargetCount; ++i)
	{
		MTargetTexture[i] = PTextureVector[i];
	}

	MDepthStencilTexture = PDepthStencilTexture;

	MViewport.TopLeftX = 0;
	MViewport.TopLeftY = 0;

	MViewport.Width = static_cast<FLOAT>(MTargetTexture[0]->GetWidth());
	MViewport.Height = static_cast<FLOAT>(MTargetTexture[0]->GetHeight());

	MViewport.MinDepth = 0.0f;
	MViewport.MaxDepth = 1.0f;
}

void FMultiRenderTarget::SetClearColorAll(const Vec4& PColor)
{
	for (auto& ClearColor : MClearColor)
	{
		ClearColor = PColor;
	}
}

void FMultiRenderTarget::SetClearColor(int PIdx, const Vec4& PColor)
{
	MClearColor[PIdx] = PColor;
}

void FMultiRenderTarget::Clear()
{
}

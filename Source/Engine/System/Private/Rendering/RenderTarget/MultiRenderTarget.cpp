#include "pch.h"
#include "Engine/System/Public/Rendering/RenderTarget/MultiRenderTarget.h"

void FMultiRenderTarget::Init(const vector<Ptr<CTexture>>& InTextureVector, const Ptr<CTexture>& InDepthStencilTexture)
{
	RenderTargetCount = static_cast<int>(InTextureVector.size());

	for (int i = 0; i < RenderTargetCount; ++i)
	{
		RenderTarget[i] = InTextureVector[i];
	}

	DepthStencilTexture = InDepthStencilTexture;

	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	Viewport.Width = static_cast<FLOAT>(RenderTarget[0]->GetWidth());
	Viewport.Height = static_cast<FLOAT>(RenderTarget[0]->GetHeight());

	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
}

/**
 * @brief Output Merger에 RenderTarget을 전달하는 함수
 */
void FMultiRenderTarget::BindTargets() const
{
	// ViewPort Setting
	CONTEXT->RSSetViewports(1, &Viewport);

	// DX11에서 처리할 수 있는 최대 갯수의 Texture View 생성
	vector<ID3D11RenderTargetView*> RenderTargetView(8, nullptr);

	// 준비해 둔 Texture 삽입
	for (int i = 0; i < RenderTargetCount; ++i)
	{
		RenderTargetView[i] = RenderTarget[i]->GetRTV().Get();
	}

	// Texture View와 준비한 Texture 갯수, 존재한다면 Depth Stencil Texture까지 Output Merger한테 전달
	if (DepthStencilTexture.Get())
	{
		CONTEXT->OMSetRenderTargets(RenderTargetCount, RenderTargetView.data(), DepthStencilTexture->GetDSV().Get());
	}
	else
	{
		CONTEXT->OMSetRenderTargets(RenderTargetCount, RenderTargetView.data(), nullptr);
	}
}

void FMultiRenderTarget::SetClearColorAll(const Vec4& InColor)
{
	for (auto& ClearColor : ClearColor)
	{
		ClearColor = InColor;
	}
}

void FMultiRenderTarget::SetClearColor(int InIdx, const Vec4& InColor)
{
	ClearColor[InIdx] = InColor;
}

void FMultiRenderTarget::ClearAll() const
{
	ClearRenderTargets();
	ClearDepthStencil();
}

void FMultiRenderTarget::ClearRenderTargets() const
{
	for (int i = 0; i < RenderTargetCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(RenderTarget[i]->GetRTV().Get(), ClearColor[i]);
	}
}

void FMultiRenderTarget::ClearDepthStencil() const
{
	if (!DepthStencilTexture.Get())
	{
		return;
	}

	CONTEXT->ClearDepthStencilView(DepthStencilTexture->GetDSV().Get(),
	                               D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

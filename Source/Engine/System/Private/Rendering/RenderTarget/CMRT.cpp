#include "pch.h"
#include "System/Public/Rendering/RenderTarget/CMRT.h"
#include "System/Public/Rendering/Device/CDevice.h"

CMRT::CMRT()
	: m_Target{}
	  , m_RTCount(0)
	  , m_DSTex(nullptr)
	  , m_ViewPort{}
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(Ptr<CTexture>* _arrTex, int _Size, Ptr<CTexture> _DSTex)
{
	m_RTCount = _Size;

	for (int i = 0; i < m_RTCount; ++i)
	{
		m_Target[i] = _arrTex[i];
	}

	m_DSTex = _DSTex;

	// ViewPort
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;

	m_ViewPort.Width = static_cast<FLOAT>(m_Target[0]->GetWidth());
	m_ViewPort.Height = static_cast<FLOAT>(m_Target[0]->GetHeight());

	m_ViewPort.MinDepth = 0;
	m_ViewPort.MaxDepth = 1;
}

void CMRT::SetClearColor(int _Idx, Vec4 _Color, bool _Independent)
{
	if (_Independent)
	{
		m_Clear[_Idx] = _Color;
	}

	else
	{
		for (int i = 0; i < 8; ++i)
		{
			m_Clear[i] = _Color;
		}
	}
}

void CMRT::Clear()
{
	ClearRenderTargets();

	ClearDepthStencil();
}

void CMRT::ClearRenderTargets()
{
	for (int i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_Target[i]->GetRTV().Get(), m_Clear[i]);
	}
}

void CMRT::ClearDepthStencil()
{
	if (nullptr == m_DSTex)
		return;

	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
								   1.f, 0);
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_Target[i]->GetRTV().Get();
	}

	CONTEXT->RSSetViewports(1, &m_ViewPort);

	if (nullptr != m_DSTex)
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, m_DSTex->GetDSV().Get());
	else
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, nullptr);
}

void CMRT::OMSetUI()
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_Target[i]->GetRTV().Get();
	}

	CONTEXT->RSSetViewports(1, &m_ViewPort);
	CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, nullptr);
}

#include "pch.h"
#include "System/Public/Rendering/Shader/CClearColorCS.h"

CClearColorCS::CClearColorCS()
	: CComputeShader(L"test_cs.cso", L"test.fx", L"CS_ClearTexture", 32, 32, 1)
{
}

CClearColorCS::~CClearColorCS() = default;

int CClearColorCS::Binding()
{
	if (nullptr == m_TargetTex)
		return E_FAIL;

	// 색칠할 Texture 를 U0 에 바인딩
	m_TargetTex->Binding_UAV_CS(0);

	// 텍스쳐 해상도 정보 전달
	m_Const.iArr[0] = static_cast<int>(m_TargetTex->GetWidth());
	m_Const.iArr[1] = static_cast<int>(m_TargetTex->GetHeight());

	// 색칠할 색상정보 전달
	m_Const.v4Arr[0] = m_ClearColor;

	return S_OK;
}

void CClearColorCS::CalcGroupCount()
{
	m_GroupX = m_TargetTex->GetWidth() / m_GroupPerThreadX;
	m_GroupY = m_TargetTex->GetHeight() / m_GroupPerThreadY;
	m_GroupZ = 1;

	if (m_TargetTex->GetWidth() % m_GroupPerThreadX)
		m_GroupX += 1;

	if (m_TargetTex->GetHeight() % m_GroupPerThreadY)
		m_GroupY += 1;
}

void CClearColorCS::Clear()
{
	m_TargetTex->Clear_UAV_CS();
	m_TargetTex = nullptr;
}

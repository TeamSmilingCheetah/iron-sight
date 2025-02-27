#include "pch.h"
#include "System/Public/Rendering/Shader/CHeightMapCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CHeightMapCS::CHeightMapCS()
	: CComputeShader(L"Shader\\heightmap.fx", "CS_HeightMap", 32, 32, 1)
{
}

CHeightMapCS::~CHeightMapCS()
{
}

int CHeightMapCS::Binding()
{
	if (nullptr == m_HeightMap)
		return E_FAIL;

	m_HeightMap->Binding_UAV_CS(0);
	m_RaycastOut->Binding_CS_SRV(20);

	m_Const.iArr[0] = m_HeightMap->GetWidth();
	m_Const.iArr[1] = m_HeightMap->GetHeight();

	m_Const.v2Arr[0] = m_BrushScale;

	if (nullptr != m_BrushTex)
	{
		m_BrushTex->Binding_SRV_CS(0);
		m_Const.bTex[0] = 1;
	}
	else
		m_Const.bTex[0] = 0;

	return S_OK;
}

void CHeightMapCS::CalcGroupCount()
{
	m_GroupX = (m_HeightMap->GetWidth() / m_GroupPerThreadX) + !!(m_HeightMap->GetWidth() %
		m_GroupPerThreadX);
	m_GroupY = (m_HeightMap->GetHeight() / m_GroupPerThreadY) + !!(m_HeightMap->GetHeight() %
		m_GroupPerThreadY);
	m_GroupZ = 1;
}

void CHeightMapCS::Clear()
{
	m_HeightMap->Clear_UAV_CS();
	m_RaycastOut->Clear_CS_SRV(20);
}

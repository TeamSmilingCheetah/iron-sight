#include "pch.h"
#include "System/Public/Rendering/Shader/CWeightMapCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CWeightMapCS::CWeightMapCS()
	: CComputeShader(L"weightmap_cs.cso", L"weightmap.fx", L"CS_WeightMap", 32, 32, 1)
	  , m_WeightMap(nullptr)
	  , m_RaycastOut(nullptr)
	  , m_WeightIdx(0)
	  , m_WeightMapWidth(0)
	  , m_WeightMapHeight(0)
{
}

CWeightMapCS::~CWeightMapCS() = default;

int CWeightMapCS::Binding()
{
	if (nullptr == m_WeightMap || nullptr == m_BrushTex || nullptr == m_RaycastOut
		|| 0 == m_WeightMapWidth || 0 == m_WeightMapHeight)
		return E_FAIL;

	m_Const.iArr[0] = m_WeightMapWidth;
	m_Const.iArr[1] = m_WeightMapHeight;
	m_Const.iArr[2] = m_WeightIdx;
	m_Const.v2Arr[0] = m_BrushScale;

	m_BrushTex->Binding_SRV_CS(0);
	m_WeightMap->Binding_CS_UAV(0);
	m_RaycastOut->Binding_CS_SRV(20);

	return S_OK;
}

void CWeightMapCS::CalculateGroupCount()
{
	m_GroupX = m_WeightMapWidth / m_GroupPerThreadX;
	m_GroupY = m_WeightMapHeight / m_GroupPerThreadY;
	m_GroupZ = 1;

	if (0 < static_cast<UINT>(m_WeightMapWidth) % m_GroupPerThreadX)
		m_GroupX += 1;

	if (0 < static_cast<UINT>(m_WeightMapHeight) % m_GroupPerThreadY)
		m_GroupY += 1;
}

void CWeightMapCS::Clear()
{
	m_BrushTex->Clear_SRV_CS();
	m_WeightMap->Clear_CS_UAV(0);
	m_RaycastOut->Clear_CS_SRV(20);
}

#include "pch.h"
#include "System/Public/Rendering/Shader/CRaycastCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CRaycastCS::CRaycastCS()
	: CComputeShader(L"raycast_cs.cso", L"raycast.fx", L"CS_RaycastBatch", 64, 1, 1)
	, MAllVertices(nullptr)
	, MAllIndices(nullptr)
	, MRaycastTasks(nullptr)
	, MResults(nullptr)
	, MTaskCount(0)
{
}

CRaycastCS::~CRaycastCS() = default;

/**
 * @brief Resource Binding Phase
 * @return 성공 여부
 */
int CRaycastCS::Binding()
{
	// Fail Condition
	if (!MAllVertices || !MAllIndices || !MRaycastTasks || !MResults)
	{
		return E_FAIL;
	}

	// Shader Resource View & Unordered Access View Binding
	MAllVertices->Binding_CS_SRV(22);
	MAllIndices->Binding_CS_SRV(23);
	MRaycastTasks->Binding_CS_SRV(25);
	MResults->Binding_CS_UAV(0);

	return S_OK;
}

/**
 * @brief Calculate Dispatch Thread Group Number
 */
void CRaycastCS::CalculateGroupCount()
{
	m_GroupX = (MTaskCount + m_GroupPerThreadX - 1) / m_GroupPerThreadX;
	m_GroupY = 1;
	m_GroupZ = 1;
}

/**
 * @brief Resource Clear
 */
void CRaycastCS::Clear()
{
	CStructuredBuffer::Clear_CS_SRV(22);
	CStructuredBuffer::Clear_CS_SRV(23);
	CStructuredBuffer::Clear_CS_SRV(25);
	CStructuredBuffer::Clear_CS_UAV(0);

	MAllVertices = nullptr;
	MAllIndices = nullptr;
	MRaycastTasks = nullptr;
	MResults = nullptr;
	MTaskCount = 0;
}

#include "pch.h"
#include "System/Public/Rendering/Shader/CRaycastCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CRaycastCS::CRaycastCS()
	: CComputeShader(L"raycast_cs.cso", L"raycast.fx", L"CS_Raycast", 32, 32, 1)
	  , m_FaceX(0)
	  , m_FaceZ(0)
	  , m_Ray{}
	  , m_OutBuffer(nullptr)
	  , m_RayInOutBuffer(nullptr)
	  , m_RayInoutCount(0)
{
}

CRaycastCS::~CRaycastCS() = default;

int CRaycastCS::Binding()
{
	if (nullptr == m_OutBuffer && nullptr == m_RayInOutBuffer)
		return E_FAIL;

	// Raycasting 을 정확하게 계산하기위해서 t0 에 높이맵도 전달
	m_Const.bTex[0] = !!m_HeightMap.Get();
	m_HeightMap->Binding_SRV_CS(0);

	m_OutBuffer->Binding_CS_UAV(0);
	m_RayInOutBuffer->Binding_CS_UAV(1);

	m_Const.iArr[0] = m_FaceX;
	m_Const.iArr[1] = m_FaceZ;
	m_Const.iArr[2] = m_RayInoutCount;

	m_Const.v4Arr[0] = m_Ray.vStart;
	m_Const.v4Arr[1] = m_Ray.vDir;

	m_Const.matArr[0] = m_LandWorldMat;

	return S_OK;
}

void CRaycastCS::CalcGroupCount()
{
	m_GroupX = (m_FaceX * 2) / m_GroupPerThreadX;
	m_GroupY = m_FaceZ / m_GroupPerThreadY;
	m_GroupZ = 1;

	if (0 < (m_FaceX * 2) % m_GroupPerThreadX)
		m_GroupX += 1;

	if (0 < m_FaceZ % m_GroupPerThreadY)
		m_GroupY += 1;
}

void CRaycastCS::Clear()
{
	m_OutBuffer->Clear_CS_UAV(0);
	m_OutBuffer = nullptr;

	m_RayInOutBuffer->Clear_CS_UAV(1);
	m_RayInOutBuffer = nullptr;

	m_HeightMap->Clear_SRV_CS();
	m_HeightMap = nullptr;
}

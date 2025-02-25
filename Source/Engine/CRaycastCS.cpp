#include "pch.h"
#include "CRaycastCS.h"

#include "CStructuredBuffer.h"

CRaycastCS::CRaycastCS()
    : CComputeShader(L"shader\\raycast.fx", "CS_Raycast", 32, 32, 1)
      , m_FaceX(0)
      , m_FaceZ(0)
      , m_Ray{}
      , m_OutBuffer(nullptr)
{
}

CRaycastCS::~CRaycastCS()
{
}


int CRaycastCS::Binding()
{
    if (nullptr == m_OutBuffer)
        return E_FAIL;

    // Raycasting 을 정확하게 계산하기위해서 t0 에 높이맵도 전달
    m_Const.bTex[0] = !!m_HeightMap.Get();
    m_HeightMap->Binding_SRV_CS(0);

    m_OutBuffer->Binding_CS_UAV(0);

    m_Const.iArr[0] = m_FaceX;
    m_Const.iArr[1] = m_FaceZ;

    m_Const.v4Arr[0] = m_Ray.vStart;
    m_Const.v4Arr[1] = m_Ray.vDir;

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

    m_HeightMap->Clear_SRV_CS();
    m_HeightMap = nullptr;
}

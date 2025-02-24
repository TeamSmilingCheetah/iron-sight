#include "pch.h"
#include "CBoneMatrixCS.h"

#include "CStructuredBuffer.h"

CBoneMatrixCS::CBoneMatrixCS()
    : CComputeShader(L"Shader\\bonemat.fx", "CS_BoneMatrix", 256, 1, 1)
      , m_FrameDataBuffer(nullptr)
      , m_OffsetMatBuffer(nullptr)
      , m_OutputBuffer(nullptr)
{
}

CBoneMatrixCS::~CBoneMatrixCS()
{
}

int CBoneMatrixCS::Binding()
{
    // 구조화버퍼 전달
    m_FrameDataBuffer->Binding_CS_SRV(16); // t16
    m_OffsetMatBuffer->Binding_CS_SRV(17); // t17
    m_OutputBuffer->Binding_CS_UAV(0); // u0

    return S_OK;
}

void CBoneMatrixCS::CalcGroupCount()
{
    UINT GroupX = (m_Const.iArr[0] / m_GroupPerThreadX) + 1;
    UINT GroupY = 1;
    UINT GroupZ = 1;
}

void CBoneMatrixCS::Clear()
{
    m_FrameDataBuffer->Clear_CS_SRV(16);
    m_OffsetMatBuffer->Clear_CS_SRV(17);
    m_OutputBuffer->Clear_CS_UAV(0);

    m_FrameDataBuffer = nullptr;
    m_OffsetMatBuffer = nullptr;
    m_OutputBuffer = nullptr;
}

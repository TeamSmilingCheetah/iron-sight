#include "pch.h"
#include "System/Public/Rendering/Shader/CBoneMatrixCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

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
	m_GroupX = (m_Const.iArr[0] / m_GroupPerThreadX) + 1;
	m_GroupY = 1;
	m_GroupZ = 1;
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

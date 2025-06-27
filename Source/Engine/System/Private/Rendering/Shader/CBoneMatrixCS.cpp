#include "pch.h"
#include "System/Public/Rendering/Shader/CBoneMatrixCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CBoneMatrixCS::CBoneMatrixCS()
	: CComputeShader(L"Shader\\bonemat.fx", "CS_BoneMatrix", 256, 1, 1)
	, m_OffsetMatBuffer(nullptr)
	, m_CurClipFrameDataBuffer(nullptr)
	, m_NextClipFrameDataBuffer(nullptr)
	, m_OutputBuffer(nullptr)
	, m_PureOutputBuffer(nullptr)
{
}

CBoneMatrixCS::~CBoneMatrixCS()
{
}

int CBoneMatrixCS::Binding()
{
	// 구조화버퍼 전달
	m_OffsetMatBuffer->Binding_CS_SRV(16); // t16
	m_CurClipFrameDataBuffer->Binding_CS_SRV(17); // t17
	if (m_NextClipFrameDataBuffer)
		m_NextClipFrameDataBuffer->Binding_CS_SRV(18); // t18
	else
		CStructuredBuffer::Clear_CS_SRV(18);
	m_OutputBuffer->Binding_CS_UAV(0); // u0
	m_PureOutputBuffer->Binding_CS_UAV(1); // u1

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
	CStructuredBuffer::Clear_CS_SRV(16);
	CStructuredBuffer::Clear_CS_SRV(17);
	CStructuredBuffer::Clear_CS_SRV(18);
	CStructuredBuffer::Clear_CS_UAV(0);
	CStructuredBuffer::Clear_CS_UAV(1);

	m_CurClipFrameDataBuffer = nullptr;
	m_OffsetMatBuffer = nullptr;
	m_OutputBuffer = nullptr;
	m_PureOutputBuffer = nullptr;
}

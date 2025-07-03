#include "pch.h"
#include "System/Public/Rendering/Shader/CCopyBoneCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CCopyBoneCS::CCopyBoneCS()
	: CComputeShader(L"copybone_cs.cso", L"copybone.fx", L"CS_CopyBoneMatrix", 256, 1, 1)
	  , m_SrcBuffer(nullptr)
	  , m_DestBuffer(nullptr)
{
}

CCopyBoneCS::~CCopyBoneCS() = default;

int CCopyBoneCS::Binding()
{
	// 구조화버퍼 전달
	m_SrcBuffer->Binding_CS_SRV(18); // t18
	m_DestBuffer->Binding_CS_UAV(0); // u0

	return S_OK;
}

void CCopyBoneCS::CalcGroupCount()
{
	// 그룹 수 계산
	int iBoneCount = m_Const.iArr[0];

	m_GroupX = iBoneCount / m_GroupPerThreadX + 1;
	m_GroupY = 1;
	m_GroupZ = 1;
}

void CCopyBoneCS::Clear()
{
	// 전달한 구조화버퍼 클리어
	m_SrcBuffer->Clear_CS_SRV(18);
	m_DestBuffer->Clear_CS_UAV(0);

	m_SrcBuffer = nullptr;
	m_DestBuffer = nullptr;
}

#include "pch.h"
#include "System/Public/Rendering/Shader/CMeshCollisionCS.h"

#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CMeshCollisionCS::CMeshCollisionCS()
	: CComputeShader(L"mesh_collision_cs.cso", L"mesh_collision.fx", L"CS_MeshCollision", 32, 32, 1)
	  , MLeftVertices(nullptr)
	  , MLeftIndices(nullptr)
	  , MRightVertices(nullptr)
	  , MRightIndices(nullptr)
	  , MCollisionCount(nullptr)
	  , MResults(nullptr)
	  , MLeftTriCount(0)
	  , MRightTriCount(0)
{
}

CMeshCollisionCS::~CMeshCollisionCS() = default;

int CMeshCollisionCS::Binding()
{
	if (!MLeftVertices || !MLeftIndices || !MRightVertices || !MRightIndices || !MCollisionCount || !MResults)
	{
		return E_FAIL;
	}

	static CConstBuffer* ConstBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MESH_COLLISION);

	MeshCollisionInfo cb = {};
	cb.LeftTriCount = MLeftTriCount;
	cb.RightTriCount = MRightTriCount;
	ConstBuffer->SetData(&cb);
	ConstBuffer->Binding_CS();

	MLeftVertices->Binding_CS_SRV(22); // t22
	MLeftIndices->Binding_CS_SRV(23); // t23
	MRightVertices->Binding_CS_SRV(24); // t24
	MRightIndices->Binding_CS_SRV(25); // t25
	MCollisionCount->Binding_CS_UAV(0); // u0
	MResults->Binding_CS_UAV(1); // u1

	return S_OK;
}

void CMeshCollisionCS::CalcGroupCount()
{
	m_GroupX = MLeftTriCount / m_GroupPerThreadX + (MLeftTriCount % m_GroupPerThreadX ? 1 : 0);
	m_GroupY = MRightTriCount / m_GroupPerThreadY + (MRightTriCount % m_GroupPerThreadY ? 1 : 0);
	m_GroupZ = 1;
}

void CMeshCollisionCS::Clear()
{
	CStructuredBuffer::Clear_CS_SRV(22);
	CStructuredBuffer::Clear_CS_SRV(23);
	CStructuredBuffer::Clear_CS_SRV(24);
	CStructuredBuffer::Clear_CS_SRV(25);
	CStructuredBuffer::Clear_CS_UAV(0);
	CStructuredBuffer::Clear_CS_UAV(1);
	MLeftVertices = nullptr;
	MLeftIndices = nullptr;
	MRightVertices = nullptr;
	MRightIndices = nullptr;
	MCollisionCount = nullptr;
	MResults = nullptr;
}

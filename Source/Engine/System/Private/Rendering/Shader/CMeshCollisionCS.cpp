#include "pch.h"
#include "System/Public/Rendering/Shader/CMeshCollisionCS.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CMeshCollisionCS::CMeshCollisionCS()
    : CComputeShader(L"mesh_collision_cs.cso", L"mesh_collision.fx", L"CS_MeshCollisionBatch", 64, 1, 1)
    , MAllVertices(nullptr)
    , MAllIndices(nullptr)
    , MCollisionTasks(nullptr)
    , MResults(nullptr)
    , MTaskCount(0)
{
}

CMeshCollisionCS::~CMeshCollisionCS() = default;

/**
 * @brief 필요한 모든 리소스를 바인딩하는 단계
 * @return 성공 여부
 */
int CMeshCollisionCS::Binding()
{
    // Fail Condition
    if (!MAllVertices || !MAllIndices || !MCollisionTasks || !MResults)
    {
       return E_FAIL;
    }

	// Shader Resource View & Unordered Access View Binding
    MAllVertices->Binding_CS_SRV(22);
    MAllIndices->Binding_CS_SRV(23);
    MCollisionTasks->Binding_CS_SRV(24);
    MResults->Binding_CS_UAV(0);

    return S_OK;
}

/**
 * @brief Calculate Dispatch Thread Group Number
 * 전체 작업에 필요한 Group의 최소 갯수로 여유 있게 세팅됨
 */
void CMeshCollisionCS::CalculateGroupCount()
{
    m_GroupX = (MTaskCount + m_GroupPerThreadX - 1) / m_GroupPerThreadX;
    m_GroupY = 1;
    m_GroupZ = 1;
}

/**
 * @brief 파이프라인에 바인딩된 모든 리소스를 해제하고 내부 상태를 초기화합니다.
 * @details 디스패치가 완료된 후에 호출되어야 합니다.
 */
void CMeshCollisionCS::Clear()
{
    // 해당 슬롯에 nullptr을 바인딩하여 파이프라인에서 리소스를 해제합니다.
    CStructuredBuffer::Clear_CS_SRV(22);
    CStructuredBuffer::Clear_CS_SRV(23);
    CStructuredBuffer::Clear_CS_SRV(24);
    CStructuredBuffer::Clear_CS_UAV(0);

    // 유효하지 않은 참조(댕글링 포인터)를 방지하기 위해 내부 포인터를 null로 만듭니다.
    MAllVertices = nullptr;
    MAllIndices = nullptr;
    MCollisionTasks = nullptr;
    MResults = nullptr;
    MTaskCount = 0;
}

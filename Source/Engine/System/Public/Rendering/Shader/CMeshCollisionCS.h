#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"

class CStructuredBuffer;

/**
 * @brief Mesh와의 충돌 여부를 확인하기 위한 Compute Shader
 * Single Dispatch를 통해 Batch Process로 충돌 감지 처리를 진행한 뒤 Result 다수를 반환
 *
 * @param MAllVertices 모든 Mesh의 모든 정점을 담고 있는 단일 버퍼
 * @param MAllIndices 모든 Mesh의 모든 인덱스를 담고 있는 단일 버퍼
 * @param MCollisionTasks 처리 작업을 정의하는 tCollisionTask를 담고 있는 버퍼
 * @param MResults 각 Task의 처리 결과를 저장하는 버퍼
 * @param MTaskCount 전체 Task의 수
 */
class CMeshCollisionCS : public CComputeShader
{
private:
    CStructuredBuffer* MAllVertices;
    CStructuredBuffer* MAllIndices;
    CStructuredBuffer* MCollisionTasks;
    CStructuredBuffer* MResults;
    UINT MTaskCount;

public:
	int Binding() override;
	void CalculateGroupCount() override;
	void Clear() override;

	// Setter
    void SetTaskBuffer(CStructuredBuffer* PTasks) { MCollisionTasks = PTasks; }
    void SetResultBuffer(CStructuredBuffer* PResults) { MResults = PResults; }
    void SetTaskCount(UINT PCount) { MTaskCount = PCount; }
	void SetVertexAndIndexBuffers(CStructuredBuffer* PVertices, CStructuredBuffer* PIndices)
    {
    	MAllVertices = PVertices;
    	MAllIndices = PIndices;
    }

	// Special Member Function
    CMeshCollisionCS();
    ~CMeshCollisionCS() override;
};

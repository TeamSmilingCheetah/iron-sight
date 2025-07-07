#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"

class CStructuredBuffer;

/**
 * @brief Mesh와의 충돌 여부를 확인하기 위한 Compute Shader
 */
class CMeshCollisionCS : public CComputeShader
{
private:
	CStructuredBuffer* MLeftVertices;
	CStructuredBuffer* MLeftIndices;
	CStructuredBuffer* MRightVertices;
	CStructuredBuffer* MRightIndices;
	CStructuredBuffer* MCollisionCount;
	CStructuredBuffer* MResults;
	INT32 MLeftTriCount;
	INT32 MRightTriCount;

public:
	void SetLeftVertices(CStructuredBuffer* PBuf) { MLeftVertices = PBuf; }
	void SetLeftIndices(CStructuredBuffer* PBuf) { MLeftIndices = PBuf; }
	void SetRightVertices(CStructuredBuffer* PBuf) { MRightVertices = PBuf; }
	void SetRightIndices(CStructuredBuffer* PBuf) { MRightIndices = PBuf; }
	void SetCount(CStructuredBuffer* PBuf) { MCollisionCount = PBuf; }
	void SetResults(CStructuredBuffer* PBuf) { MResults = PBuf; }
	void SetTriCounts(INT32 PLeft, INT32 PRight) { MLeftTriCount = PLeft; MRightTriCount = PRight; }

	int Binding() override;
	void CalculateGroupCount() override;
	void Clear() override;

	CMeshCollisionCS();
	~CMeshCollisionCS() override;
};

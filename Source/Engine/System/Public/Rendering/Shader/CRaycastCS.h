#pragma once
#include "Engine/System/Public/Rendering/Shader/CComputeShader.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

class CStructuredBuffer;

class CRaycastCS :
    public CComputeShader
{
    UINT	m_FaceX;
    UINT	m_FaceZ;
    tRay	m_Ray;
	Matrix	m_LandWorldMat;

    Ptr<CTexture> m_HeightMap;
    CStructuredBuffer* m_OutBuffer;

	CStructuredBuffer* m_RayInOutBuffer;
	UINT				m_RayInoutCount;

public:
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void SetRayInfo(const tRay& _ray) { m_Ray = _ray; }
    void SetFace(UINT _FaceX, UINT _FaceZ) { m_FaceX = _FaceX, m_FaceZ = _FaceZ; }
    void SetOutBuffer(CStructuredBuffer* _Buffer) { m_OutBuffer = _Buffer; }
	void SetRayInOutBuffer(CStructuredBuffer* _Buffer) { m_RayInOutBuffer = _Buffer; }
	void SetRayInOutCount(UINT _idx) { m_RayInoutCount = _idx; }
	void SetLandWorldMat(const Matrix& _Mat) { m_LandWorldMat = _Mat; }

    int Binding() override;
    void CalcGroupCount() override;
    void Clear() override;

    CRaycastCS();
    ~CRaycastCS() override;
};

#pragma once
#include "CAsset.h"

#include "CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
    ComPtr<ID3D11Buffer> IB;
    D3D11_BUFFER_DESC tIBDesc;
    UINT IdxCount;
    void* IdxSysMem;
};

class CMesh :
    public CAsset
{
    ComPtr<ID3D11Buffer> m_VB;
    D3D11_BUFFER_DESC m_VBDesc;
    UINT m_VtxCount;
    Vtx* m_VtxSysMem;

    // 하나의 버텍스 버퍼에 여러개의 인덱스 버퍼가 연결
    vector<tIndexInfo> m_vecIdxInfo;

    // Animation3D 정보
    vector<tMTAnimClip> m_vecAnimClip;
    vector<tMTBone> m_vecBones;

    CStructuredBuffer* m_BoneFrameData; // 전체 본 프레임 정보(크기, 이동, 회전) (프레임 개수만큼)
    CStructuredBuffer* m_BoneInverse; // 각 뼈의 상쇄(무효화) 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)

    int Load(const wstring& _FilePath) override;

public:
    int Save(const wstring& _FilePath) override;

    static CMesh* CreateFromContainer(CFBXLoader& _loader);
    int Create(Vtx* _pVtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount);

    UINT GetVertexCount() { return m_VtxCount; }
    UINT GetSubsetCount() { return static_cast<UINT>(m_vecIdxInfo.size()); }
    void* GetVtxSysMem() { return m_VtxSysMem; }

    const vector<tMTBone>* GetBones() { return &m_vecBones; }
    UINT GetBoneCount() { return static_cast<UINT>(m_vecBones.size()); }
    const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }
    CStructuredBuffer* GetBoneFrameDataBuffer() { return m_BoneFrameData; } // 전체 본 프레임 정보
    CStructuredBuffer* GetBoneInverseBuffer() { return m_BoneInverse; } // 각 Bone 의 Inverse 행렬

    void Binding(UINT _Subset);
    void Render(UINT _Subset);
    void Render_Particle(UINT _Count);

    CLONE_DISABLE(CMesh);
    CMesh(bool _bEngineRes = false);
    ~CMesh() override;
};

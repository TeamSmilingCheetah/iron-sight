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

    // �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
    vector<tIndexInfo> m_vecIdxInfo;

    // Animation3D ����
    vector<tMTAnimClip> m_vecAnimClip;
    vector<tMTBone> m_vecBones;

    CStructuredBuffer* m_BoneFrameData; // ��ü �� ������ ����(ũ��, �̵�, ȸ��) (������ ������ŭ)
    CStructuredBuffer* m_BoneInverse; // �� ���� ���(��ȿȭ) ���(�� ���� ��ġ�� �ǵ����� ���) (1�� ¥��)

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
    CStructuredBuffer* GetBoneFrameDataBuffer() { return m_BoneFrameData; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneInverseBuffer() { return m_BoneInverse; } // �� Bone �� Inverse ���

    void Binding(UINT _Subset);
    void Render(UINT _Subset);
    void Render_Particle(UINT _Count);

    CLONE_DISABLE(CMesh);
    CMesh(bool _bEngineRes = false);
    ~CMesh() override;
};

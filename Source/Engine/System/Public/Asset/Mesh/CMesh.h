#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/System/Public/Rendering/Tool/FBX/CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
	ComPtr<ID3D11Buffer> IB;
	D3D11_BUFFER_DESC IBDesc;
	UINT IdxCount;
	void* IdxSysMem;
};

class CMesh :
	public CAsset
{
private:
	ComPtr<ID3D11Buffer> m_VB;
	D3D11_BUFFER_DESC m_VBDesc;
	UINT m_VtxCount;
	Vtx* m_VtxSysMem;

	// 하나의 버텍스 버퍼에 여러개의 인덱스 버퍼가 연결
	vector<tIndexInfo> m_vecIdxInfo;

private:
	int Load(const wstring& _RelativePath) override;

public:
	int Save(const wstring& _RelativePath) override;

	static CMesh* CreateFromContainer(CFBXLoader& _loader, int _ContainerIdx);
	int Create(Vtx* _pVtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount);

	UINT GetVertexCount() const { return m_VtxCount; }
	UINT GetSubsetCount() const { return static_cast<UINT>(m_vecIdxInfo.size()); }
	void* GetVtxSysMem() const { return m_VtxSysMem; }

	// Binding
	void Binding(UINT _Subset);
	void Binding_Inst(UINT _Subset);

	void Render(UINT _Subset);
	void Render_Cluster_Instancing(UINT _Count);
	void Render_Object_Instancing(UINT _iSubset);

public:
	CLONE_DISABLE(CMesh);
	CMesh(bool _bEngineRes = false);
	~CMesh() override;
};

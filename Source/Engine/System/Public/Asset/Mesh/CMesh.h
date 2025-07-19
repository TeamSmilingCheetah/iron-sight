#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CFBXLoader;
struct tIndexInfo;

/**
 * @brief 메시(3D Mesh) 데이터를 관리하는 클래스
 *
 * @var m_VB         버텍스 버퍼 (GPU용, ID3D11Buffer)
 * @var m_VBDesc     버텍스 버퍼의 DirectX11 버퍼 설명 구조체
 * @var m_VtxCount   버텍스 개수
 * @var m_VtxSysMem  버텍스 데이터의 시스템 메모리 포인터 (Vtx 배열)
 * @var m_vecIdxInfo 인덱스 버퍼 정보의 벡터, 서브셋 단위 관리
 * @var m_BoundMin   메시의 로컬 바운딩 박스 최소 좌표
 * @var m_BoundMax   메시의 로컬 바운딩 박스 최대 좌표
 */
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

	// 로컬 바운드 정보
	Vec3 m_BoundMin;
	Vec3 m_BoundMax;

private:
	int Load(const wstring& _RelativePath) override;

public:
	int Save(const wstring& _RelativePath) override;

	static CMesh* CreateFromContainer(CFBXLoader& _loader, int _ContainerIdx);
	int Create(Vtx* _pVtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount);

	UINT GetVertexCount() const { return m_VtxCount; }
	UINT GetSubsetCount() const { return static_cast<UINT>(m_vecIdxInfo.size()); }
	vector<tIndexInfo>& GetIndexInfo() { return m_vecIdxInfo; }
	void* GetVtxSysMem() const { return m_VtxSysMem; }

	// Local Bound
	array<Vec3, 2> GetLocalBound() const { return { m_BoundMin, m_BoundMax }; }
	void CalculateLocalBound();

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

#include "pch.h"
#include "System/Public/Asset/Mesh/CMesh.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Manager/RenderManager.h"
#include "System/Public/Rendering/Buffer/CInstancingBuffer.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Device/CDevice.h"
#include "System/Public/Rendering/Tool/FBX/CFBXLoader.h"

CMesh::CMesh(bool _bEngineRes)
	: CAsset(MESH, _bEngineRes)
	, m_VBDesc{}
	, m_VtxCount(0)
	, m_VtxSysMem(nullptr)
	, m_BoundMin(FLT_MAX, FLT_MAX, FLT_MAX)
	, m_BoundMax(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}

CMesh::~CMesh()
{
	DELETE_ARR(m_VtxSysMem);

	for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		if (nullptr != m_vecIdxInfo[i].IdxSysMem)
			delete m_vecIdxInfo[i].IdxSysMem;
	}
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader, int _ContainerIdx)
{
	const tContainer* container = &_loader.GetContainer(_ContainerIdx);

	UINT iVtxCount = static_cast<UINT>(container->vecPos.size());

	D3D11_BUFFER_DESC tVtxDesc = {};

	tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
	tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
		tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
	Vtx* pSys = (Vtx*)tSub.pSysMem;
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUV = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights = container->vecWeights[i];
		pSys[i].vIndices = container->vecIndices[i];
	}

	ComPtr<ID3D11Buffer> pVB = nullptr;
	if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
	{
		return nullptr;
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_VB = pVB;
	pMesh->m_VBDesc = tVtxDesc;
	pMesh->m_VtxCount = iVtxCount;
	pMesh->m_VtxSysMem = pSys;

	// 인덱스 정보
	UINT iIdxBufferCount = static_cast<UINT>(container->vecIdx.size());
	D3D11_BUFFER_DESC tIdxDesc = {};

	pMesh->m_vecIdxInfo.reserve(iIdxBufferCount);

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIndexInfo info{};

		if (container->vecIdx[i].size() > 0)
		{
			tIdxDesc.ByteWidth = static_cast<UINT>(container->vecIdx[i].size()) * sizeof(UINT);
			// Index Format 이 R32_UINT 이기 때문
			tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
			if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
				tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			void* pSysMem = malloc(tIdxDesc.ByteWidth);
			memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
			tSub.pSysMem = pSysMem;

			ComPtr<ID3D11Buffer> pIB = nullptr;
			if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
			{
				return nullptr;
			}

			info.IB = pIB;
			info.IBDesc = tIdxDesc;
			info.IdxCount = static_cast<UINT>(container->vecIdx[i].size());
			info.IdxSysMem = pSysMem;
		}
		else
		{
			// TEST : index가 0개면 모든 걸 null로 등록.
			// 나중에 바인딩에서 문제 생길 수도 있음
			info = {};
		}

		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Local Bound 계산
	pMesh->CalculateLocalBound();

	return pMesh;
}

int CMesh::Create(Vtx* _Vtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount)
{
	m_VtxCount = _VtxCount;

	m_VtxSysMem = new Vtx[m_VtxCount];
	memcpy(m_VtxSysMem, _Vtx, sizeof(Vtx) * m_VtxCount);

	// 정점 데이터를 SysMem -> GPU Mem 로 이동
	m_VBDesc.ByteWidth = sizeof(Vtx) * _VtxCount;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

	// 생성시킬 버퍼의 초기데이터 전달용
	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _Vtx;

	// 버텍스 용도의 ID3D11Buffer 객체 생성
	if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSub, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 인덱스 버퍼 생성
	tIndexInfo IndexInfo = {};
	IndexInfo.IdxCount = _IdxCount;

	IndexInfo.IBDesc.ByteWidth = sizeof(UINT) * _IdxCount;

	// 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
	IndexInfo.IBDesc.CPUAccessFlags = 0;
	IndexInfo.IBDesc.Usage = D3D11_USAGE_DEFAULT;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	IndexInfo.IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexInfo.IBDesc.MiscFlags = 0;
	IndexInfo.IBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체
	tSub.pSysMem = _pIdx;

	if (FAILED(DEVICE->CreateBuffer(&IndexInfo.IBDesc, &tSub, IndexInfo.IB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// 시스템 메모리에 저장
	IndexInfo.IdxSysMem = new UINT[IndexInfo.IdxCount];
	memcpy(IndexInfo.IdxSysMem, _pIdx, sizeof(UINT) * IndexInfo.IdxCount);

	m_vecIdxInfo.push_back(IndexInfo);

	// Local Bound 계산
	CalculateLocalBound();

	return S_OK;
}

void CMesh::Binding(UINT _iSubset)
{
	UINT stride = sizeof(Vtx);
	UINT offset = 0;
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::Binding_Inst(UINT _iSubset)
{
	if (_iSubset >= m_vecIdxInfo.size())
		assert(nullptr);

	ID3D11Buffer* arrBuffer[2] = {m_VB.Get(), CInstancingBuffer::GetInst()->GetBuffer().Get()};
	UINT iStride[2] = {sizeof(Vtx), sizeof(tInstancingData)};
	UINT iOffset[2] = {0, 0};

	CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::Render(UINT _Subset)
{
	BatchKey key;
	key.Mesh = this;
	key.SubsetIndex = _Subset;

	// Material은 호출하는 쪽에서 설정해야 함
	tInstancingData instData = {};
	instData.matWorld = g_Trans.matWorld;
	instData.matWV = g_Trans.matWV;
	instData.matWVP = g_Trans.matWVP;

	// CRenderMgr에서 현재 렌더링 상태를 가져옵니다.
	RENDER_STATE renderState = FRenderManager::GetInst()->GetCurrentRenderState();

	if (renderState == RENDER_STATE::DEFERRED)
	{
		FRenderManager::GetInst()->AddDeferredBatch(key, instData);
	}
	else if (renderState == RENDER_STATE::FORWARD)
	{
		FRenderManager::GetInst()->AddForwardBatch(key, instData);
	}
	else if (renderState == RENDER_STATE::RENDER_TRANSPARENT)
	{
		FRenderManager::GetInst()->AddTransparentBatch(key, instData);
	}
}

void CMesh::Render_Cluster_Instancing(UINT _Count)
{
	// [여기 확인] 클러스터 렌더링을 위한 바인딩을 수행합니다.
	// 일반적으로 클러스터 렌더링은 단일 서브셋(0번)의 메쉬를 사용합니다.
	Binding(0);

	// [여기 확인] GPU 인스턴싱을 사용하여 동일한 메쉬를 여러 번 효율적으로 렌더링합니다.
	// _Count 만큼의 인스턴스를 생성하며, 각 인스턴스는 고유한 월드 행렬 등을 가질 수 있습니다.
	// 이는 드로우콜 횟수를 크게 줄여 성능을 향상시킵니다.
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].IdxCount, _Count, 0, 0, 0);
}

void CMesh::Render_Object_Instancing(UINT _Subset)
{
	// [여기 확인] 오브젝트별 인스턴싱을 위해 정점 버퍼와 인스턴스 버퍼를 함께 바인딩합니다.
	// 인스턴스 버퍼에는 각 인스턴스의 위치, 회전, 크기 등의 정보가 담겨있습니다.
	Binding_Inst(_Subset);

	// [여기 확인] 특정 서브셋의 메쉬를 인스턴싱하여 렌더링합니다.
	// CInstancingBuffer에서 현재 프레임에 렌더링할 인스턴스의 개수를 가져와 사용합니다.
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_Subset].IdxCount
		, CInstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

int CMesh::Save(const wstring& _RelativePath)
{
	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// 키값, 상대 경로
	SaveWString(GetName(), pFile);
	SaveWString(GetKey(), pFile);
	SaveWString(GetRelativePath(), pFile);

	// 정점 데이터 저장
	fwrite(&m_VtxCount, sizeof(UINT), 1, pFile);
	int iByteSize = m_VBDesc.ByteWidth;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_VtxSysMem, iByteSize, 1, pFile);

	// 인덱스 정보
	UINT iMtrlCount = static_cast<UINT>(m_vecIdxInfo.size());
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].IdxSysMem
		       , m_vecIdxInfo[i].IdxCount * sizeof(UINT)
		       , 1, pFile);
	}
	fclose(pFile);

	return S_OK;
}

int CMesh::Load(const wstring& _strFilePath)
{
	// 읽기모드로 파일열기
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값, 상대경로
	wstring strName, strKey, strRelativePath;
	LoadWString(strName, pFile);
	LoadWString(strKey, pFile);
	LoadWString(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);

	// 정점데이터
	fread(&m_VtxCount, sizeof(UINT), 1, pFile);
	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_VtxSysMem = static_cast<Vtx*>(malloc(iByteSize));
	fread(m_VtxSysMem, 1, iByteSize, pFile);


	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth = iByteSize;
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = m_VtxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		UINT iByteWidth = info.IdxCount * sizeof(UINT);

		void* pSysMem = malloc(iByteWidth);
		info.IdxSysMem = pSysMem;
		fread(info.IdxSysMem, iByteWidth, 1, pFile);

		tSubData.pSysMem = info.IdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.IBDesc, &tSubData, info.IB.GetAddressOf())))
		{
			assert(nullptr);
		}

		m_vecIdxInfo.push_back(info);
	}
	fclose(pFile);

	// Local Bound 세팅
	CalculateLocalBound();

	return S_OK;
}

void CMesh::CalculateLocalBound()
{
	if (!m_VtxSysMem || !m_VtxCount)
	{
		return;
	}

	// 모든 버텍스에 대해 바운딩 박스 계산
	for (UINT i = 0; i < m_VtxCount; ++i)
	{
		// 최소값 갱신
		m_BoundMin.x = min(m_BoundMin.x, m_VtxSysMem[i].vPos.x);
		m_BoundMin.y = min(m_BoundMin.y, m_VtxSysMem[i].vPos.y);
		m_BoundMin.z = min(m_BoundMin.z, m_VtxSysMem[i].vPos.z);

		// 최대값 갱신
		m_BoundMax.x = max(m_BoundMax.x, m_VtxSysMem[i].vPos.x);
		m_BoundMax.y = max(m_BoundMax.y, m_VtxSysMem[i].vPos.y);
		m_BoundMax.z = max(m_BoundMax.z, m_VtxSysMem[i].vPos.z);
	}

	// 모든 정점을 순회하고도 갱신이 이루어지지 않은 경우
	// TODO(KHJ): 완벽한 갱신 요소 체크는 아님, 필요 시에 빡빡하게 설정할 것
	if (m_BoundMin.x == FLT_MAX || m_BoundMax.x == -FLT_MAX)
	{
		assert("If The Update Is Not Done Even After Traversing All Vertices, Enter This Point.");
	}
}

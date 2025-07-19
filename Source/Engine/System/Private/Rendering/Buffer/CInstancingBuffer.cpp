#include "pch.h"
#include "System/Public/Rendering/Buffer/CInstancingBuffer.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Shader/CCopyBoneCS.h"

CInstancingBuffer::CInstancingBuffer()
	: m_MaxCount(10)
	  , m_AnimInstCount(0)
	  , m_BoneBuffer(nullptr)
{
	m_BoneBuffer = new CStructuredBuffer;
}

CInstancingBuffer::~CInstancingBuffer()
{
	if (nullptr != m_BoneBuffer)
		delete m_BoneBuffer;
}

void CInstancingBuffer::Init()
{
	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = sizeof(tInstancingData) * m_MaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_InstancingBuffer)))
		assert(NULL);

	m_CopyShader = new CCopyBoneCS;
}

void CInstancingBuffer::SetData()
{
	if (m_vecData.size() > m_MaxCount)
	{
		Resize(static_cast<UINT>(m_vecData.size()));
	}

	D3D11_MAPPED_SUBRESOURCE tMap = {};

	CONTEXT->Map(m_InstancingBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
	memcpy(tMap.pData, &m_vecData[0], sizeof(tInstancingData) * m_vecData.size());
	CONTEXT->Unmap(m_InstancingBuffer.Get(), 0);

	// 본 행렬정보 메모리 복사
	if (m_vecBoneFinalMat.empty())
		return;

	// 모든 오브젝트의 finalbonemat을 저장할 buffer 생성
	UINT iBufferSize = static_cast<UINT>(m_vecBoneFinalMat.size() * m_vecBoneFinalMat[0]->GetBufferSize());
	if (m_BoneBuffer->GetBufferSize() < iBufferSize)
	{
		m_BoneBuffer->Create(m_vecBoneFinalMat[0]->GetElementSize()
		                     , m_vecBoneFinalMat[0]->GetElementCount() * static_cast<UINT>(m_vecBoneFinalMat.size()), SB_TYPE::SRV_UAV, false,
		                     nullptr);
	}

	// 복사용 컴퓨트 쉐이더 실행
	// instancing 순서로 각 오브젝트의 프레임 정보에 대한 finalbonemat을 append 하는 형식으로 복사
	// ( 1번 오브젝트의 finalbonemat | 2번 오브젝트의 finalbonemat | ... )
	UINT iBoneCount = m_vecBoneFinalMat[0]->GetElementCount();
	m_CopyShader->SetBoneCount(iBoneCount);

	for (UINT i = 0; i < static_cast<UINT>(m_vecBoneFinalMat.size()); ++i)
	{
		m_CopyShader->SetRowIndex(i);
		m_CopyShader->SetSourceBuffer(m_vecBoneFinalMat[i]);
		m_CopyShader->SetDestBuffer(m_BoneBuffer);
		m_CopyShader->Execute();
	}

	// Bone 정보 전달 레지스터
	m_BoneBuffer->Binding(17);
}

void CInstancingBuffer::AddInstancingBoneMat(CStructuredBuffer* _pBuffer)
{
	++m_AnimInstCount;
	m_vecBoneFinalMat.push_back(_pBuffer);
}

void CInstancingBuffer::Resize(UINT _iCount)
{
	m_InstancingBuffer = nullptr;

	m_MaxCount = _iCount;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = sizeof(tInstancingData) * m_MaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_InstancingBuffer)))
		assert(NULL);
}

#include "pch.h"
#include "Engine/System/Public/Asset/Animation/CSkeleton.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Engine/System/Public/Rendering/Tool/FBX/CFBXLoader.h"

struct tBone;

CSkeleton::CSkeleton(bool _bEngineRes)
	: CAsset(SKELETON, _bEngineRes)
	, m_BoneInvBuffer(nullptr)
	, m_BoneParentBuffer(nullptr)
{
}

CSkeleton::~CSkeleton()
{
	if (nullptr != m_BoneInvBuffer)
		delete m_BoneInvBuffer;

	if (nullptr != m_BoneParentBuffer)
		delete m_BoneParentBuffer;
}


int CSkeleton::FindBoneIndex(const wstring& _BoneName) const
{
	for (int i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i].strBoneName == _BoneName)
			return i;
	}

	// 검색 실패시 존재하지않는 인덱스 -1 반환
	return -1;
}

const tMTBone* CSkeleton::FindBone(const wstring& _BoneName) const
{
	// 존재하는지 검색
	int index = FindBoneIndex(_BoneName);
	if (index >= 0)
	{
		// 존재하면 해당 본 데이터 반환
		return &m_vecBones[index];
	}

	return nullptr;
}

Ptr<CSkeleton> CSkeleton::LoadFromFBX(CFBXLoader& _loader)
{
	Ptr<CSkeleton> pSkeleton = nullptr;

	const vector<tBone*>& vecBones = _loader.GetBones();

	// Bone이 없으면 nullptr 리턴
	if (vecBones.empty())
		return nullptr;

	// 이미 (루트 본 이름으로) 등록된 skeleton이라면 로드하지 않음
	wstring fileName = L"Skeleton\\" + vecBones[0]->strBoneName + L".bone";

	pSkeleton = CAssetMgr::GetInst()->Load<CSkeleton>(fileName);
	if (pSkeleton != nullptr)
		return pSkeleton;

	pSkeleton = new CSkeleton;
	pSkeleton->SetName(fileName);
	pSkeleton->SetKey(fileName);
	pSkeleton->SetRelativePath(fileName);

	for (UINT i = 0; i < vecBones.size(); ++i)
	{
		tMTBone bone = {};
		bone.iDepth = vecBones[i]->iDepth;
		bone.iParentIndx = vecBones[i]->iParentIndx;
		bone.matBone = GetMatrixFromFbxMatrix(vecBones[i]->matBone);
		bone.matOffset = GetMatrixFromFbxMatrix(vecBones[i]->matOffset);
		bone.strBoneName = vecBones[i]->strBoneName;


		pSkeleton->m_vecBones.push_back(bone);
		pSkeleton->m_vecOffset.push_back(bone.matOffset);
		pSkeleton->m_vecParent.push_back(bone.iParentIndx);
	}

	// 모델 공간 바인드 포즈 계산
	vector<Matrix> bindModel(vecBones.size());
	for (UINT i = 0; i < vecBones.size(); ++i)
	{
		bindModel[i] = XMMatrixInverse(nullptr, pSkeleton->m_vecBones[i].matOffset);
	}

	// 로컬 바인드 포즈 계산
	pSkeleton->m_vecBindLocal.resize(vecBones.size());
	for (UINT i = 0; i < vecBones.size(); ++i)
	{
		int p = pSkeleton->m_vecBones[i].iParentIndx;
		if (p < 0)
		{
			pSkeleton->m_vecBindLocal[i] = bindModel[i];
		}
		else
		{
			Matrix parentInv = XMMatrixInverse(nullptr, bindModel[p]);
			pSkeleton->m_vecBindLocal[i] = parentInv * bindModel[i];
		}
	}

	pSkeleton->m_BoneInvBuffer = new CStructuredBuffer;
	pSkeleton->m_BoneInvBuffer->Create(sizeof(Matrix), static_cast<UINT>(pSkeleton->m_vecOffset.size()), SRV_ONLY,
		false, pSkeleton->m_vecOffset.data());

	pSkeleton->m_BoneParentBuffer = new CStructuredBuffer;
	pSkeleton->m_BoneParentBuffer->Create(sizeof(int), static_cast<UINT>(pSkeleton->m_vecParent.size()), SRV_ONLY,
		false, pSkeleton->m_vecParent.data());

	// AssetMgr 등록 (key 값 설정)
	CAssetMgr::GetInst()->AddAsset<CSkeleton>(pSkeleton->GetName(), pSkeleton);

	// Save (relative path 설정)
	pSkeleton->Save(fileName);

	return pSkeleton;
}

int CSkeleton::Save(const wstring& _RelativePath)
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

	UINT iCount = static_cast<UINT>(m_vecBones.size());
	fwrite(&iCount, sizeof(int), 1, pFile);

	// Bone Info
	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecBones[i].strBoneName, pFile);
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBindLocal[i], sizeof(Matrix), 1, pFile);
	}

	fclose(pFile);

	return S_OK;
}


int CSkeleton::Load(const wstring& _strFilePath)
{
	// 읽기모드로 파일열기
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	if (pFile == nullptr)
		return E_FAIL;

	// 키값, 상대경로
	wstring strName, strKey, strRelativePath;
	LoadWString(strName, pFile);
	LoadWString(strKey, pFile);
	LoadWString(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);

	UINT iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);
	m_vecBindLocal.resize(iCount);

	for (UINT i = 0; i < iCount; ++i)
	{
		LoadWString(m_vecBones[i].strBoneName, pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecBindLocal[i], sizeof(Matrix), 1, pFile);

		m_vecOffset.push_back(m_vecBones[i].matOffset);
		m_vecParent.push_back(m_vecBones[i].iParentIndx);
	}

	// structured buffer 생성
	m_BoneInvBuffer = new CStructuredBuffer;
	m_BoneInvBuffer->Create(sizeof(Matrix), iCount, SRV_ONLY, false, m_vecOffset.data());

	m_BoneParentBuffer = new CStructuredBuffer;
	m_BoneParentBuffer->Create(sizeof(int), iCount, SRV_ONLY, false, m_vecParent.data());

	fclose(pFile);

	return S_OK;
}

#include "pch.h"
#include "System/Public/Asset/Animation/CSkeleton.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

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


Ptr<CSkeleton> CSkeleton::LoadFromFBX(CFBXLoader& _loader)
{
	Ptr<CSkeleton> pSkeleton = nullptr;

	const vector<tBone*>& vecBones = _loader.GetBones();

	// Bone이 없으면 nullptr 리턴
	if (vecBones.empty())
		return nullptr;

	// 이미 (루트 본 이름으로) 등록된 skeleton이라면 로드하지 않음
	pSkeleton = CAssetMgr::GetInst()->FindAsset<CSkeleton>(vecBones[0]->strBoneName);
	if (pSkeleton != nullptr)
		return pSkeleton;

	pSkeleton = new CSkeleton;
	pSkeleton->SetName(vecBones[0]->strBoneName + L".bone");

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

	pSkeleton->m_BoneInvBuffer = new CStructuredBuffer;
	pSkeleton->m_BoneInvBuffer->Create(sizeof(Matrix), static_cast<UINT>(pSkeleton->m_vecOffset.size()), SRV_ONLY,
		false, pSkeleton->m_vecOffset.data());

	pSkeleton->m_BoneParentBuffer = new CStructuredBuffer;
	pSkeleton->m_BoneParentBuffer->Create(sizeof(int), static_cast<UINT>(pSkeleton->m_vecParent.size()), SRV_ONLY,
		false, pSkeleton->m_vecParent.data());

	// AssetMgr 등록 (key 값 설정)
	CAssetMgr::GetInst()->AddAsset<CSkeleton>(pSkeleton->GetName(), pSkeleton);

	// Save (relative path 설정)
	wstring strFilePath = L"Skeleton\\" + pSkeleton->GetName();
	pSkeleton->Save(strFilePath);

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
	}

	fclose(pFile);

	return S_OK;
}


int CSkeleton::Load(const wstring& _strFilePath)
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

	UINT iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	for (UINT i = 0; i < iCount; ++i)
	{
		LoadWString(m_vecBones[i].strBoneName, pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);

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

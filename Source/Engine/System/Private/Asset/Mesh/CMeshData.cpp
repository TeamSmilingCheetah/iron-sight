#include "pch.h"
#include "System/Public/Asset/Mesh/CMeshData.h"

#include "Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CPathMgr.h"

CMeshData::CMeshData(bool _Engine)
	: CAsset(MESH_DATA, false)
{
}

CMeshData::~CMeshData()
{
}

CGameObject* CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->SetLayerIdx(0);	// 일단 0으로 지정

	int meshCnt = static_cast<int>(m_vecMesh.size());
	assert(meshCnt > 0);

	// Animation이 없고, 메쉬가 한 개라면 오브젝트를 바로 리턴
	if (m_vecAnimSet.empty() && meshCnt == 1)
	{
		pNewObj->AddComponent(new CMeshRender);
		pNewObj->SetName(m_vecMesh[0]->GetKey());

		pNewObj->MeshRender()->SetMesh(m_vecMesh[0]);

		for (UINT i = 0; i < m_vecMtrlSet[0].size(); ++i)
		{
			pNewObj->MeshRender()->SetMaterial(m_vecMtrlSet[0][i], i);
		}
	}

	// Animation이 있거나, mesh가 여러 개라면
	else
	{
		// Animation이 있다면, 
		if (!m_vecAnimSet.empty())
		{
			// 1. 부모에 애니메이터 추가
			CAnimator3D* pAnimator = new CAnimator3D;
			pNewObj->AddComponent(pAnimator);

			pAnimator->SetAnimClip(m_vecAnimSet);
			pAnimator->SetCurClip(0);

			// 2. skeleton에 대응되는 오브젝트를 자식에 넣음
			UINT BoneCount = pAnimator->GetBoneCount();

			vector<CGameObject*> vecBoneObject(BoneCount);
			for (int i = 0; i < BoneCount; ++i)
			{
				vecBoneObject[i] = new CGameObject;
				vecBoneObject[i]->Transform()->SetManualUpdate(true);
			}

			// 부모 자식 관계 세팅. 1번부터 하는 이유는 0번은 자기자신을 부모라고 하고 있음
			// 모든 애니메이션이 같은 skeleton을 공유한다는 전제.
			const vector<tMTBone>* vecBones = m_vecAnimSet[0]->GetBones();
			for (int i = 1; i < BoneCount; ++i)
			{
				int parentIdx = vecBones->at(i).iParentIndx;

				vecBoneObject[i]->SetName(vecBones->at(i).strBoneName);
				vecBoneObject[parentIdx]->AddChild(vecBoneObject[i]);
			}

			// 루트를 자식으로 추가
			vecBoneObject[0]->SetName(vecBones->at(0).strBoneName);
			pNewObj->AddChild(vecBoneObject[0]);

			// Animator에 넘겨줌
			pAnimator->SetBoneObject(move(vecBoneObject));
		}
		
		// 빈 오브젝트 밑에 mesh에 대응되는 오브젝트를 추가
		for (int idx = 0; idx < meshCnt; ++idx)
		{
			CGameObject* childObj = new CGameObject;
			childObj->AddComponent(new CMeshRender);
			childObj->SetName(m_vecMesh[idx]->GetKey());

			childObj->MeshRender()->SetMesh(m_vecMesh[idx]);

			for (UINT i = 0; i < m_vecMtrlSet[idx].size(); ++i)
			{
				childObj->MeshRender()->SetMaterial(m_vecMtrlSet[idx][i], i);

				// Animation이 있다면,
				if (!m_vecAnimSet.empty())
				{
					childObj->MeshRender()->SetSkinRender(true);
				}
			}

			pNewObj->AddChild(childObj);
		}
	}

	return pNewObj;
}

void CMeshData::Instantiate(CGameObject* _Obj)
{
	int meshCnt = static_cast<int>(m_vecMesh.size());
	assert(meshCnt > 0);

	// 1개의 메쉬로 이루어진 경우 - 오브젝트에 바로 meshrenderer 부착
	if (m_vecMesh.size() == 1)
	{
		_Obj->AddComponent(new CMeshRender);

		_Obj->MeshRender()->SetMesh(m_vecMesh[0]);

		for (UINT i = 0; i < m_vecMtrlSet[0].size(); ++i)
		{
			_Obj->MeshRender()->SetMaterial(m_vecMtrlSet[0][i], i);
		}

		// Animation 파트 추가
		if (!m_vecAnimSet.empty())
		{
			CAnimator3D* pAnimator = new CAnimator3D;
			_Obj->AddComponent(pAnimator);
			_Obj->MeshRender()->SetSkinRender(true);
			
			// FIXME : 본이 여러 개인 경우 이상해질 수 잇음 (서로 다른 bone을 사용하는 애니메이션이 로드 될 수 있음)
			pAnimator->SetAnimClip(m_vecAnimSet);
			pAnimator->SetCurClip(0);
		}
	}

	// 2개 이상의 메쉬로 이루어진 경우. 한 empty 오브젝트 밑에 메쉬마다 자식으로 생성
	else
	{
		// Animation 파트 추가
		if (!m_vecAnimSet.empty())
		{
			CAnimator3D* pAnimator = new CAnimator3D;
			_Obj->AddComponent(pAnimator);

			// FIXME : 본이 여러 개인 경우 이상해질 수 잇음 (서로 다른 bone을 사용하는 애니메이션이 로드 될 수 있음)
			pAnimator->SetAnimClip(m_vecAnimSet);
			pAnimator->SetCurClip(0);
		}

		for (int idx = 0; idx < meshCnt; ++idx)
		{
			CGameObject* childObj = new CGameObject;
			childObj->AddComponent(new CMeshRender);
			childObj->SetName(m_vecMesh[idx]->GetKey());

			childObj->Transform()->SetRelativeScale(Vec3(1.f, 1.f, 1.f));
			childObj->MeshRender()->SetMesh(m_vecMesh[idx]);

			for (UINT i = 0; i < m_vecMtrlSet[idx].size(); ++i)
			{
				childObj->MeshRender()->SetMaterial(m_vecMtrlSet[idx][i], i);

				// 애니메이션을 사용한다면
				if (!m_vecAnimSet.empty())
				{
					childObj->MeshRender()->SetSkinRender(true);
				}
			}

			_Obj->AddChild(childObj);
		}
	}
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _RelativePath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _RelativePath;

	CFBXLoader loader;
	loader.Init();
	loader.LoadFbx(strFullPath);

	CMeshData* pMeshData = new CMeshData(true);

	int ContainerCnt = loader.GetContainerCount();

	for (int idx = 0; idx < ContainerCnt; ++idx)
	{
		// 메쉬 가져오기
		Ptr<CMesh> pMesh = nullptr;
		const tContainer& Container = loader.GetContainer(idx);
		pMesh = CMesh::CreateFromContainer(loader, idx);

		// AssetMgr 에 메쉬 등록
		if (nullptr != pMesh)
		{
			wstring strMeshKey = path(strFullPath).stem();
			strMeshKey += L"_";
			strMeshKey += Container.strName;
			strMeshKey += L".mesh";

			if (nullptr == CAssetMgr::GetInst()->FindAsset<CMesh>(strMeshKey))
			{
				// 메시를 실제 파일로 저장
				CAssetMgr::GetInst()->AddAsset<CMesh>(strMeshKey, pMesh);

				wstring strFilePath = L"Mesh\\" + strMeshKey;
				pMesh->Save(strFilePath);
			}
		}
		else
		{
			// FBX 를 로딩했는데 거기서 나온 메쉬가 이미 메모리(에셋매니저) 에 로딩 되어있는 메쉬였다면
			// pMesh 가 스마트 포인터라서 별다른 조치를 하지 않아도 만들어진 메쉬가 삭제될것
		}

		// 메테리얼 가져오기
		vector<Ptr<CMaterial>> vecMtrl;
		vecMtrl.reserve(Container.vecMtrl.size());

		for (UINT i = 0; i < Container.vecMtrl.size(); ++i)
		{
			// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
			Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(Container.vecMtrl[i].strMtrlName);
			assert(pMtrl.Get());

			vecMtrl.push_back(pMtrl);
		}

		pMeshData->m_vecMesh.push_back(pMesh);
		pMeshData->m_vecMtrlSet.push_back(vecMtrl);
	}

	// Skeleton 로드하기
	CSkeleton::LoadFromFBX(loader);

	// Animation 로드하기
	pMeshData->m_vecAnimSet = CAnimation::LoadFromFBX(loader);

	return pMeshData;
}


int CMeshData::Save(const wstring& _RelativePath)
{
	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// Mesh 개수 저장
	int meshCnt = static_cast<int>(m_vecMesh.size());
	fwrite(&meshCnt, sizeof(int), 1, pFile);

	for (int idx = 0; idx < meshCnt; ++idx)
	{
		// Mesh Key 저장
		// Mesh Data 저장
		SaveAssetRef(m_vecMesh[idx], pFile);

		// material 정보 저장
		UINT iMtrlCount = static_cast<UINT>(m_vecMtrlSet[idx].size());
		fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

		UINT i = 0;
		wstring strMtrlPath = CPathMgr::GetInst()->GetContentPath();
		strMtrlPath += L"Material\\";

		for (; i < iMtrlCount; ++i)
		{
			if (nullptr == m_vecMtrlSet[idx][i])
				continue;

			// Material 인덱스, Key, Path 저장
			fwrite(&i, sizeof(UINT), 1, pFile);
			SaveAssetRef(m_vecMtrlSet[idx][i], pFile);
		}

		i = -1; // 마감 값 (underflow)
		fwrite(&i, sizeof(UINT), 1, pFile);
	}

	// Animation set 저장
	UINT animCount = static_cast<UINT>(m_vecAnimSet.size());
	fwrite(&animCount, sizeof(UINT), 1, pFile);

	for (UINT i = 0; i < animCount; ++i)
	{
		SaveAssetRef(m_vecAnimSet[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}

int CMeshData::Load(const wstring& _FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	assert(pFile);

	int meshCnt = 0;
	fread(&meshCnt, sizeof(int), 1, pFile);
	m_vecMesh.resize(meshCnt);
	m_vecMtrlSet.resize(meshCnt);

	for (int idx = 0; idx < meshCnt; ++idx)
	{
		// Mesh Load
		LoadAssetRef(m_vecMesh[idx], pFile);
		assert(m_vecMesh[idx].Get());

		// material 정보 읽기
		UINT iMtrlCount = 0;
		fread(&iMtrlCount, sizeof(UINT), 1, pFile);

		m_vecMtrlSet[idx].resize(iMtrlCount);

		UINT i = 0;
		while (i != (UINT)-1)
		{
			fread(&i, sizeof(UINT), 1, pFile);
			if (i == -1)
				break;

			LoadAssetRef(m_vecMtrlSet[idx][i], pFile);
		}
	}

	// Animation set 로드
	UINT animCount = 0;
	fread(&animCount, sizeof(UINT), 1, pFile);

	m_vecAnimSet.resize(animCount);

	for (UINT i = 0; i < animCount; ++i)
	{
		LoadAssetRef(m_vecAnimSet[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}

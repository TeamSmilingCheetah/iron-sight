#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAnimator3D.h"

#include "CFBXLoader.h"

CMeshData::CMeshData(bool _Engine)
    : CAsset(MESH_DATA, false)
{
}

CMeshData::~CMeshData()
{
}

CGameObject* CMeshData::Instantiate()
{
    auto pNewObj = new CGameObject;
    //pNewObj->AddComponent(new CTransform);
    pNewObj->AddComponent(new CMeshRender);

    pNewObj->MeshRender()->SetMesh(m_pMesh);

    for (UINT i = 0; i < m_vecMtrl.size(); ++i)
    {
        pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
    }

    // Animation 파트 추가
    if (false == m_pMesh->IsAnimMesh())
        return pNewObj;

    auto pAnimator = new CAnimator3D;
    pNewObj->AddComponent(pAnimator);

    pAnimator->SetBones(m_pMesh->GetBones());
    pAnimator->SetAnimClip(m_pMesh->GetAnimClip());

    return pNewObj;
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _RelativePath)
{
    wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
    strFullPath += _RelativePath;

    CFBXLoader loader;
    loader.init();
    loader.LoadFbx(strFullPath);

    // 메쉬 가져오기
    Ptr<CMesh> pMesh = nullptr;
    pMesh = CMesh::CreateFromContainer(loader);

    // AssetMgr 에 메쉬 등록
    if (nullptr != pMesh)
    {
        wstring strMeshKey = L"Mesh\\";
        strMeshKey += path(strFullPath).stem();
        strMeshKey += L".mesh";

        if (nullptr == CAssetMgr::GetInst()->FindAsset<CMesh>(strMeshKey))
        {
            // 메시를 실제 파일로 저장
            CAssetMgr::GetInst()->AddAsset<CMesh>(strMeshKey, pMesh);
            pMesh->Save(CPathMgr::GetInst()->GetContentPath() + strMeshKey);
        }
    }
    else
    {
        // FBX 를 로딩했는데 거기서 나온 메쉬가 이미 메모리(에셋매니저) 에 로딩 되어있는 메쉬였다면
        // pMesh 가 스마트 포인터라서 별다른 조치를 하지 않아도 만들어진 메쉬가 삭제될것
    }

    vector<Ptr<CMaterial>> vecMtrl;

    // 메테리얼 가져오기
    for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
    {
        // 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
        Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(
            loader.GetContainer(0).vecMtrl[i].strMtrlName);
        assert(pMtrl.Get());

        vecMtrl.push_back(pMtrl);
    }

    auto pMeshData = new CMeshData(true);
    pMeshData->m_pMesh = pMesh;
    pMeshData->m_vecMtrl = vecMtrl;

    return pMeshData;
}


int CMeshData::Save(const wstring& _RelativePath)
{
    SetRelativePath(_RelativePath);

    wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _RelativePath;

    FILE* pFile = nullptr;
    errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
    assert(pFile);

    // Mesh Key 저장	
    // Mesh Data 저장
    SaveAssetRef(m_pMesh, pFile);

    // material 정보 저장
    UINT iMtrlCount = static_cast<UINT>(m_vecMtrl.size());
    fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

    UINT i = 0;
    wstring strMtrlPath = CPathMgr::GetInst()->GetContentPath();
    strMtrlPath += L"material\\";

    for (; i < iMtrlCount; ++i)
    {
        if (nullptr == m_vecMtrl[i])
            continue;

        // Material 인덱스, Key, Path 저장
        fwrite(&i, sizeof(UINT), 1, pFile);
        SaveAssetRef(m_vecMtrl[i], pFile);
    }

    i = -1; // 마감 값
    fwrite(&i, sizeof(UINT), 1, pFile);

    fclose(pFile);

    return S_OK;
}

int CMeshData::Load(const wstring& _FilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _FilePath.c_str(), L"rb");

    assert(pFile);

    // Mesh Load
    LoadAssetRef(m_pMesh, pFile);
    assert(m_pMesh.Get());

    // material 정보 읽기
    UINT iMtrlCount = 0;
    fread(&iMtrlCount, sizeof(UINT), 1, pFile);

    m_vecMtrl.resize(iMtrlCount);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        UINT idx = -1;
        fread(&idx, 4, 1, pFile);
        if (idx == -1)
            break;

        wstring strKey, strPath;

        Ptr<CMaterial> pMtrl;
        LoadAssetRef(pMtrl, pFile);

        m_vecMtrl[i] = pMtrl;
    }

    fclose(pFile);

    return S_OK;
}

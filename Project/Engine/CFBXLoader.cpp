#include "pch.h"
#include "CFBXLoader.h"
#include "CMesh.h"

#include "CAssetMgr.h"
#include "CPathMgr.h"


CFBXLoader::CFBXLoader()
    : m_pManager(nullptr)
      , m_pScene(nullptr)
      , m_pImporter(nullptr)
{
}

CFBXLoader::~CFBXLoader()
{
    m_pScene->Destroy();
    m_pManager->Destroy();

    for (size_t i = 0; i < m_vecBone.size(); ++i)
    {
        if (nullptr != m_vecBone[i])
            delete m_vecBone[i];
    }

    for (size_t i = 0; i < m_vecAnimClip.size(); ++i)
    {
        if (nullptr != m_vecAnimClip[i])
            delete m_vecAnimClip[i];
    }

    for (int i = 0; i < m_arrAnimName.Size(); ++i)
    {
        if (nullptr != m_arrAnimName[i])
            delete m_arrAnimName[i];
    }
}

void CFBXLoader::init()
{
    m_pManager = FbxManager::Create();
    if (nullptr == m_pManager)
        assert(NULL);

    FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);
    m_pManager->SetIOSettings(pIOSettings);

    m_pScene = FbxScene::Create(m_pManager, "");
    if (nullptr == m_pScene)
        assert(NULL);
}

void CFBXLoader::LoadFbx(const wstring& _strPath)
{
    m_vecContainer.clear();

    m_pImporter = FbxImporter::Create(m_pManager, "");

    //wstring str = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(strName.c_str());
    string strPath(_strPath.begin(), _strPath.end());

    if (!m_pImporter->Initialize(strPath.c_str(), -1, m_pManager->GetIOSettings()))
        assert(nullptr);

    m_pImporter->Import(m_pScene);

    /*FbxAxisSystem originAxis = FbxAxisSystem::eMax;
    originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem DesireAxis = FbxAxisSystem::DirectX;
    DesireAxis.ConvertScene(m_pScene);
    originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();*/

    m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

    // Bone ���� �б�
    LoadSkeleton(m_pScene->GetRootNode());

    // Animation �̸����� 
    m_pScene->FillAnimStackNameArray(m_arrAnimName);

    // Animation Clip ����
    LoadAnimationClip();

    // �ﰢȭ(Triangulate)
    Triangulate(m_pScene->GetRootNode());

    // �޽� ������ ���
    LoadMeshDataFromNode(m_pScene->GetRootNode());

    m_pImporter->Destroy();

    // �ʿ��� �ؽ��� �ε�
    LoadTexture();

    // �ʿ��� ���׸��� ����
    CreateMaterial();
}

void CFBXLoader::LoadMeshDataFromNode(FbxNode* _pNode)
{
    // ����� �޽����� �б�
    FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();


    if (pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType())
    {
        FbxAMatrix matGlobal = _pNode->EvaluateGlobalTransform();
        matGlobal.GetR();

        FbxMesh* pMesh = _pNode->GetMesh();
        if (nullptr != pMesh)
            LoadMesh(pMesh);
    }

    // �ش� ����� �������� �б�
    UINT iMtrlCnt = _pNode->GetMaterialCount();
    if (iMtrlCnt > 0)
    {
        for (UINT i = 0; i < iMtrlCnt; ++i)
        {
            FbxSurfaceMaterial* pMtrlSur = _pNode->GetMaterial(i);
            LoadMaterial(pMtrlSur);
        }
    }

    // �ڽ� ��� ���� �б�
    int iChildCnt = _pNode->GetChildCount();
    for (int i = 0; i < iChildCnt; ++i)
    {
        LoadMeshDataFromNode(_pNode->GetChild(i));
    }
}

void CFBXLoader::LoadMesh(FbxMesh* _pFbxMesh)
{
    m_vecContainer.push_back(tContainer{});
    tContainer& Container = m_vecContainer[m_vecContainer.size() - 1];

    string strName = _pFbxMesh->GetName();
    Container.strName = wstring(strName.begin(), strName.end());

    int iVtxCnt = _pFbxMesh->GetControlPointsCount();
    Container.Resize(iVtxCnt);

    FbxVector4* pFbxPos = _pFbxMesh->GetControlPoints();

    for (int i = 0; i < iVtxCnt; ++i)
    {
        Container.vecPos[i].x = static_cast<float>(pFbxPos[i].mData[0]);
        Container.vecPos[i].y = static_cast<float>(pFbxPos[i].mData[2]);
        Container.vecPos[i].z = static_cast<float>(pFbxPos[i].mData[1]);
    }

    // ������ ����
    int iPolyCnt = _pFbxMesh->GetPolygonCount();

    // ������ ���� ( ==> SubSet ���� ==> Index Buffer Count)
    int iMtrlCnt = _pFbxMesh->GetNode()->GetMaterialCount();
    Container.vecIdx.resize(iMtrlCnt);

    // ���� ������ ���� subset �� �˱����ؼ�...
    FbxGeometryElementMaterial* pMtrl = _pFbxMesh->GetElementMaterial();

    // �������� �����ϴ� ���� ����
    int iPolySize = _pFbxMesh->GetPolygonSize(0);
    if (3 != iPolySize)
        assert(NULL); // Polygon ���� ������ 3���� �ƴ� ���

    UINT arrIdx[3] = {};
    UINT iVtxOrder = 0; // ������ ������ �����ϴ� ����

    for (int i = 0; i < iPolyCnt; ++i)
    {
        for (int j = 0; j < iPolySize; ++j)
        {
            // i ��° �����￡, j ��° ����
            int iIdx = _pFbxMesh->GetPolygonVertex(i, j);
            arrIdx[j] = iIdx;

            GetTangent(_pFbxMesh, &Container, iIdx, iVtxOrder);
            GetBinormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
            GetNormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
            GetUV(_pFbxMesh, &Container, iIdx, _pFbxMesh->GetTextureUVIndex(i, j));

            ++iVtxOrder;
        }
        UINT iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
        Container.vecIdx[iSubsetIdx].push_back(arrIdx[0]);
        Container.vecIdx[iSubsetIdx].push_back(arrIdx[2]);
        Container.vecIdx[iSubsetIdx].push_back(arrIdx[1]);
    }

    LoadAnimationData(_pFbxMesh, &Container);
}

void CFBXLoader::LoadMaterial(FbxSurfaceMaterial* _pMtrlSur)
{
    tFbxMaterial tMtrlInfo{};

    string str = _pMtrlSur->GetName();
    tMtrlInfo.strMtrlName = wstring(str.begin(), str.end());

    // Diff
    tMtrlInfo.tMtrl.vDiff = GetMtrlData(_pMtrlSur
                                        , FbxSurfaceMaterial::sDiffuse
                                        , FbxSurfaceMaterial::sDiffuseFactor);

    // Amb
    tMtrlInfo.tMtrl.vAmb = GetMtrlData(_pMtrlSur
                                       , FbxSurfaceMaterial::sAmbient
                                       , FbxSurfaceMaterial::sAmbientFactor);

    // Spec
    tMtrlInfo.tMtrl.vSpec = GetMtrlData(_pMtrlSur
                                        , FbxSurfaceMaterial::sSpecular
                                        , FbxSurfaceMaterial::sSpecularFactor);

    // Emisv
    tMtrlInfo.tMtrl.vEmv = GetMtrlData(_pMtrlSur
                                       , FbxSurfaceMaterial::sEmissive
                                       , FbxSurfaceMaterial::sEmissiveFactor);

    // Texture Name
    tMtrlInfo.strDiff = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sDiffuse);
    tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sNormalMap);
    tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sSpecular);
    tMtrlInfo.strEmis = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sEmissive);


    m_vecContainer.back().vecMtrl.push_back(tMtrlInfo);
}

void CFBXLoader::GetTangent(FbxMesh* _pMesh
                            , tContainer* _pContainer
                            , int _iIdx /*�ش� ������ �ε���*/
                            , int _iVtxOrder /*������ ������ �����ϴ� ����*/)
{
    int iTangentCnt = _pMesh->GetElementTangentCount();
    if (1 != iTangentCnt)
        assert(NULL); // ���� 1���� �����ϴ� ź��Ʈ ������ 2�� �̻��̴�.

    // ź��Ʈ data �� ���� �ּ�
    FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
    UINT iTangentIdx = 0;

    if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
    {
        if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
            iTangentIdx = _iVtxOrder;
        else
            iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
    }
    else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
    {
        if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
            iTangentIdx = _iIdx;
        else
            iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
    }

    FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

    _pContainer->vecTangent[_iIdx].x = static_cast<float>(vTangent.mData[0]);
    _pContainer->vecTangent[_iIdx].y = static_cast<float>(vTangent.mData[2]);
    _pContainer->vecTangent[_iIdx].z = static_cast<float>(vTangent.mData[1]);
}

void CFBXLoader::GetBinormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder)
{
    int iBinormalCnt = _pMesh->GetElementBinormalCount();
    if (1 != iBinormalCnt)
        assert(NULL); // ���� 1���� �����ϴ� ������ ������ 2�� �̻��̴�.

    // ������ data �� ���� �ּ�
    FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
    UINT iBinormalIdx = 0;

    if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
    {
        if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
            iBinormalIdx = _iVtxOrder;
        else
            iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
    }
    else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
    {
        if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
            iBinormalIdx = _iIdx;
        else
            iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
    }

    FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

    _pContainer->vecBinormal[_iIdx].x = static_cast<float>(vBinormal.mData[0]);
    _pContainer->vecBinormal[_iIdx].y = static_cast<float>(vBinormal.mData[2]);
    _pContainer->vecBinormal[_iIdx].z = static_cast<float>(vBinormal.mData[1]);
}

void CFBXLoader::GetNormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder)
{
    int iNormalCnt = _pMesh->GetElementNormalCount();
    if (1 != iNormalCnt)
        assert(NULL); // ���� 1���� �����ϴ� ������ ������ 2�� �̻��̴�.

    // ������ data �� ���� �ּ�
    FbxGeometryElementNormal* pNormal = _pMesh->GetElementNormal();
    UINT iNormalIdx = 0;

    if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
    {
        if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
            iNormalIdx = _iVtxOrder;
        else
            iNormalIdx = pNormal->GetIndexArray().GetAt(_iVtxOrder);
    }
    else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
    {
        if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
            iNormalIdx = _iIdx;
        else
            iNormalIdx = pNormal->GetIndexArray().GetAt(_iIdx);
    }

    FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

    _pContainer->vecNormal[_iIdx].x = static_cast<float>(vNormal.mData[0]);
    _pContainer->vecNormal[_iIdx].y = static_cast<float>(vNormal.mData[2]);
    _pContainer->vecNormal[_iIdx].z = static_cast<float>(vNormal.mData[1]);
}

void CFBXLoader::GetUV(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iUVIndex)
{
    FbxGeometryElementUV* pUV = _pMesh->GetElementUV();

    UINT iUVIdx = 0;
    if (pUV->GetReferenceMode() == FbxGeometryElement::eDirect)
        iUVIdx = _iIdx;
    else
        iUVIdx = pUV->GetIndexArray().GetAt(_iIdx);

    iUVIdx = _iUVIndex;
    FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
    _pContainer->vecUV[_iIdx].x = static_cast<float>(vUV.mData[0]);
    _pContainer->vecUV[_iIdx].y = 1.f - static_cast<float>(vUV.mData[1]);
    // fbx uv ��ǥ��� ���ϴ��� 0,0
}

Vec4 CFBXLoader::GetMtrlData(FbxSurfaceMaterial* _pSurface
                             , const char* _pMtrlName
                             , const char* _pMtrlFactorName)
{
    FbxDouble3 vMtrl;
    FbxDouble dFactor = 0.;

    FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
    FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

    if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
    {
        vMtrl = tMtrlProperty.Get<FbxDouble3>();
        dFactor = tMtrlFactorProperty.Get<FbxDouble>();
    }

    auto vRetVal = Vec4(static_cast<float>(vMtrl.mData[0]) * static_cast<float>(dFactor),
                        static_cast<float>(vMtrl.mData[1]) * static_cast<float>(dFactor),
                        static_cast<float>(vMtrl.mData[2]) * static_cast<float>(dFactor),
                        static_cast<float>(dFactor));
    return vRetVal;
}

wstring CFBXLoader::GetMtrlTextureName(FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty)
{
    string strName;

    FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
    if (TextureProperty.IsValid())
    {
        UINT iCnt = TextureProperty.GetSrcObjectCount();

        if (1 <= iCnt)
        {
            FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
            if (nullptr != pFbxTex)
                strName = pFbxTex->GetFileName();
        }
    }

    return wstring(strName.begin(), strName.end());
}

void CFBXLoader::LoadTexture()
{
    path path_content = CPathMgr::GetInst()->GetContentPath();

    path path_fbx_texture = path_content.wstring() + L"texture\\FBXTexture\\";
    if (false == exists(path_fbx_texture))
    {
        create_directory(path_fbx_texture);
    }

    path path_origin;
    path path_filename;
    path path_dest;

    for (UINT i = 0; i < m_vecContainer.size(); ++i)
    {
        for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
        {
            vector<path> vecPath;
            vecPath.push_back(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
            vecPath.push_back(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
            vecPath.push_back(m_vecContainer[i].vecMtrl[j].strSpec.c_str());
            vecPath.push_back(m_vecContainer[i].vecMtrl[j].strEmis.c_str());

            for (size_t k = 0; k < vecPath.size(); ++k)
            {
                if (vecPath[k].filename().empty())
                    continue;

                path_origin = vecPath[k];
                path_filename = vecPath[k].filename();
                path_dest = path_fbx_texture.wstring() + path_filename.wstring();

                if (false == exists(path_dest))
                {
                    copy(path_origin, path_dest);
                }

                path_dest = CPathMgr::GetInst()->GetRelativePath(path_dest);
                CAssetMgr::GetInst()->Load<CTexture>(path_dest, path_dest);

                switch (k)
                {
                case 0: m_vecContainer[i].vecMtrl[j].strDiff = path_dest;
                    break;
                case 1: m_vecContainer[i].vecMtrl[j].strNormal = path_dest;
                    break;
                case 2: m_vecContainer[i].vecMtrl[j].strSpec = path_dest;
                    break;
                case 3: m_vecContainer[i].vecMtrl[j].strEmis = path_dest;
                    break;
                }
            }
        }
        path_origin = path_origin.parent_path();
        remove_all(path_origin);
    }
}

void CFBXLoader::CreateMaterial()
{
    wstring strMtrlName;
    wstring strPath;

    for (UINT i = 0; i < m_vecContainer.size(); ++i)
    {
        for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
        {
            // Material �̸�����
            strMtrlName = m_vecContainer[i].vecMtrl[j].strMtrlName;
            if (strMtrlName.empty())
                strMtrlName = path(m_vecContainer[i].vecMtrl[j].strDiff).stem();

            strPath = L"material\\";
            strPath += strMtrlName + L".mtrl";

            // ���� �̸�
            m_vecContainer[i].vecMtrl[j].strMtrlName = strPath;

            // �̹� �ε��� �����̸� �ε��� ���� ���
            Ptr<CMaterial> pMaterial = CAssetMgr::GetInst()->FindAsset<CMaterial>(strPath);
            if (nullptr != pMaterial)
                continue;

            pMaterial = new CMaterial;

            // ����ΰ� �� Ű
            pMaterial->SetKey(strPath);
            pMaterial->SetRelativePath(strPath);

            pMaterial->SetShader(
                CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"Std3D_DeferredShader"));

            wstring strTexKey = m_vecContainer[i].vecMtrl[j].strDiff;
            Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strTexKey);
            if (nullptr != pTex)
                pMaterial->SetTexParam(TEX_0, pTex);

            strTexKey = m_vecContainer[i].vecMtrl[j].strNormal;
            pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strTexKey);
            if (nullptr != pTex)
                pMaterial->SetTexParam(TEX_1, pTex);

            strTexKey = m_vecContainer[i].vecMtrl[j].strSpec;
            pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strTexKey);
            if (nullptr != pTex)
                pMaterial->SetTexParam(TEX_2, pTex);

            strTexKey = m_vecContainer[i].vecMtrl[j].strEmis;
            pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strTexKey);
            if (nullptr != pTex)
                pMaterial->SetTexParam(TEX_3, pTex);

            pMaterial->SetMaterialCoefficient(
                m_vecContainer[i].vecMtrl[j].tMtrl.vDiff
                , m_vecContainer[i].vecMtrl[j].tMtrl.vSpec
                , m_vecContainer[i].vecMtrl[j].tMtrl.vAmb
                , m_vecContainer[i].vecMtrl[j].tMtrl.vEmv);

            CAssetMgr::GetInst()->AddAsset<CMaterial>(pMaterial->GetKey(), pMaterial.Get());
            pMaterial->Save(CPathMgr::GetInst()->GetContentPath() + strPath);
        }
    }
}

void CFBXLoader::LoadSkeleton(FbxNode* _pNode)
{
    int iChildCount = _pNode->GetChildCount();

    LoadSkeleton_Re(_pNode, 0, 0, -1);
}

void CFBXLoader::LoadSkeleton_Re(FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx)
{
    FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

    if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        auto pBone = new tBone;

        string strBoneName = _pNode->GetName();

        pBone->strBoneName = wstring(strBoneName.begin(), strBoneName.end());
        pBone->iDepth = _iDepth++;
        pBone->iParentIndx = _iParentIdx;

        m_vecBone.push_back(pBone);
    }

    int iChildCount = _pNode->GetChildCount();
    for (int i = 0; i < iChildCount; ++i)
    {
        LoadSkeleton_Re(_pNode->GetChild(i), _iDepth, static_cast<int>(m_vecBone.size()), _iIdx);
    }
}

void CFBXLoader::LoadAnimationClip()
{
    int iAnimCount = m_arrAnimName.GetCount();

    for (int i = 0; i < iAnimCount; ++i)
    {
        FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());


        //FbxAnimEvaluator* pevaluator = m_pScene->GetAnimationEvaluator();
        //m_pScene->SetCurrentAnimationStack();


        if (!pAnimStack)
            continue;

        auto pAnimClip = new tAnimClip;

        string strClipName = pAnimStack->GetName();
        pAnimClip->strName = wstring(strClipName.begin(), strClipName.end());

        FbxTakeInfo* pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
        pAnimClip->tStartTime = pTakeInfo->mLocalTimeSpan.GetStart();
        pAnimClip->tEndTime = pTakeInfo->mLocalTimeSpan.GetStop();

        pAnimClip->eMode = m_pScene->GetGlobalSettings().GetTimeMode();
        pAnimClip->llTimeLength = pAnimClip->tEndTime.GetFrameCount(pAnimClip->eMode) - pAnimClip->
            tStartTime.GetFrameCount(pAnimClip->eMode);


        m_vecAnimClip.push_back(pAnimClip);
    }
}

void CFBXLoader::Triangulate(FbxNode* _pNode)
{
    FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

    if (pAttr &&
        (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh
            || pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs
            || pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
    {
        FbxGeometryConverter converter(m_pManager);
        converter.Triangulate(pAttr, true);
    }

    int iChildCount = _pNode->GetChildCount();

    for (int i = 0; i < iChildCount; ++i)
    {
        Triangulate(_pNode->GetChild(i));
    }
}

void CFBXLoader::LoadAnimationData(FbxMesh* _pMesh, tContainer* _pContainer)
{
    // Animation Data �ε��� �ʿ䰡 ����
    int iSkinCount = _pMesh->GetDeformerCount(FbxDeformer::eSkin);
    if (iSkinCount <= 0 || m_vecAnimClip.empty())
        return;

    _pContainer->bAnimation = true;

    // Skin ������ŭ �ݺ����ϸ� �д´�.	
    for (int i = 0; i < iSkinCount; ++i)
    {
        auto pSkin = static_cast<FbxSkin*>(_pMesh->GetDeformer(i, FbxDeformer::eSkin));

        if (pSkin)
        {
            FbxSkin::EType eType = pSkin->GetSkinningType();
            if (FbxSkin::eRigid == eType || FbxSkin::eLinear)
            {
                // Cluster �� ���´�
                // Cluster == Joint == ����
                int iClusterCount = pSkin->GetClusterCount();

                for (int j = 0; j < iClusterCount; ++j)
                {
                    FbxCluster* pCluster = pSkin->GetCluster(j);

                    if (!pCluster->GetLink())
                        continue;

                    // ���� �� �ε����� ���´�.
                    int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
                    if (-1 == iBoneIdx)
                        assert(NULL);

                    FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

                    // Weights And Indices ������ �д´�.
                    LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);

                    // Bone �� OffSet ��� ���Ѵ�.
                    LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer);

                    // Bone KeyFrame �� ����� ���Ѵ�.
                    LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx,
                                          _pContainer);
                }
            }
        }
    }
    CheckWeightAndIndices(_pMesh, _pContainer);
}


void CFBXLoader::CheckWeightAndIndices(FbxMesh* _pMesh, tContainer* _pContainer)
{
    auto iter = _pContainer->vecWI.begin();

    int iVtxIdx = 0;
    for (iter; iter != _pContainer->vecWI.end(); ++iter, ++iVtxIdx)
    {
        if (iter->size() > 1)
        {
            // ����ġ �� ������ �������� ����
            sort(iter->begin(), iter->end()
                 , [](const tWeightsAndIndices& left, const tWeightsAndIndices& right)
                 {
                     return left.dWeight > right.dWeight;
                 }
            );

            double dWeight = 0.f;
            for (UINT i = 0; i < iter->size(); ++i)
            {
                dWeight += (*iter)[i].dWeight;
            }

            // ����ġ�� ���� 1�� �Ѿ�� ó���κп� �����ش�.
            double revision = 0.f;
            if (dWeight > 1.0)
            {
                revision = 1.0 - dWeight;
                (*iter)[0].dWeight += revision;
            }

            if (iter->size() >= 4)
            {
                iter->erase(iter->begin() + 4, iter->end());
            }
        }

        // ���� ������ ��ȯ, 
        float fWeights[4] = {};
        float fIndices[4] = {};

        for (UINT i = 0; i < iter->size(); ++i)
        {
            fWeights[i] = static_cast<float>((*iter)[i].dWeight);
            fIndices[i] = static_cast<float>((*iter)[i].iBoneIdx);
        }

        memcpy(&_pContainer->vecWeights[iVtxIdx], fWeights, sizeof(Vec4));
        memcpy(&_pContainer->vecIndices[iVtxIdx], fIndices, sizeof(Vec4));
    }
}

void CFBXLoader::LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster
                                       , const FbxAMatrix& _matNodeTransform, int _iBoneIdx,
                                       tContainer* _pContainer)
{
    if (m_vecAnimClip.empty())
        return;

    FbxVector4 v1 = {1, 0, 0, 0};
    FbxVector4 v2 = {0, 0, 1, 0};
    FbxVector4 v3 = {0, 1, 0, 0};
    FbxVector4 v4 = {0, 0, 0, 1};
    FbxAMatrix matReflect;
    matReflect.mData[0] = v1;
    matReflect.mData[1] = v2;
    matReflect.mData[2] = v3;
    matReflect.mData[3] = v4;

    m_vecBone[_iBoneIdx]->matBone = _matNodeTransform;

    FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

    FbxLongLong llStartFrame = m_vecAnimClip[0]->tStartTime.GetFrameCount(eTimeMode);
    FbxLongLong llEndFrame = m_vecAnimClip[0]->tEndTime.GetFrameCount(eTimeMode);

    for (FbxLongLong i = llStartFrame; i < llEndFrame; ++i)
    {
        tKeyFrame tFrame = {};
        FbxTime tTime = 0;

        tTime.SetFrame(i, eTimeMode);

        FbxAMatrix matFromNode = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
        FbxAMatrix matCurTrans = matFromNode.Inverse() * _pCluster->GetLink()->
            EvaluateGlobalTransform(tTime);
        matCurTrans = matReflect * matCurTrans * matReflect;

        tFrame.dTime = tTime.GetSecondDouble();
        tFrame.matTransform = matCurTrans;

        m_vecBone[_iBoneIdx]->vecKeyFrame.push_back(tFrame);
    }
}

void CFBXLoader::LoadOffsetMatrix(FbxCluster* _pCluster
                                  , const FbxAMatrix& _matNodeTransform
                                  , int _iBoneIdx, tContainer* _pContainer)
{
    FbxAMatrix matClusterTrans;
    FbxAMatrix matClusterLinkTrans;

    _pCluster->GetTransformMatrix(matClusterTrans);
    _pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

    // Reflect Matrix
    FbxVector4 V0 = {1, 0, 0, 0};
    FbxVector4 V1 = {0, 0, 1, 0};
    FbxVector4 V2 = {0, 1, 0, 0};
    FbxVector4 V3 = {0, 0, 0, 1};

    FbxAMatrix matReflect;
    matReflect[0] = V0;
    matReflect[1] = V1;
    matReflect[2] = V2;
    matReflect[3] = V3;

    FbxAMatrix matOffset;
    matOffset = matClusterLinkTrans.Inverse() * matClusterTrans * _matNodeTransform;
    matOffset = matReflect * matOffset * matReflect;

    m_vecBone[_iBoneIdx]->matOffset = matOffset;
}


void CFBXLoader::LoadWeightsAndIndices(FbxCluster* _pCluster
                                       , int _iBoneIdx
                                       , tContainer* _pContainer)
{
    int iIndicesCount = _pCluster->GetControlPointIndicesCount();

    for (int i = 0; i < iIndicesCount; ++i)
    {
        tWeightsAndIndices tWI = {};

        // �� �������� �� �ε��� ������, ����ġ ���� �˸���.
        tWI.iBoneIdx = _iBoneIdx;
        tWI.dWeight = _pCluster->GetControlPointWeights()[i];

        int iVtxIdx = _pCluster->GetControlPointIndices()[i];

        _pContainer->vecWI[iVtxIdx].push_back(tWI);
    }
}


int CFBXLoader::FindBoneIndex(string _strBoneName)
{
    auto strBoneName = wstring(_strBoneName.begin(), _strBoneName.end());

    for (UINT i = 0; i < m_vecBone.size(); ++i)
    {
        if (m_vecBone[i]->strBoneName == strBoneName)
            return i;
    }

    return -1;
}

FbxAMatrix CFBXLoader::GetTransform(FbxNode* _pNode)
{
    const FbxVector4 vT = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 vR = _pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    const FbxVector4 vS = _pNode->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(vT, vR, vS);
}

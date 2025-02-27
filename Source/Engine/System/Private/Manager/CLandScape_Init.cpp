#include "pch.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"

void CLandScape::Init()
{
    CreateMesh();

    CreateMaterial();

    CreateComputeShader();

    // BrushTexture 추가
    AddBrushTexture(
        CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_GlowScene_2.png"));
    AddBrushTexture(
        CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash_0.png"));
    AddBrushTexture(
        CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_HitFlash02.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\TX_Twirl02.png"));
    AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"Texture\\brush\\FX_Flare.png"));

    // Raycasting 결과를 받는 용도의 구조화버퍼
    m_RaycastOut = new CStructuredBuffer;
    m_RaycastOut->Create(sizeof(tRaycastOut), 1, SRV_UAV, true);
}

void CLandScape::CreateMesh()
{
    Ptr<CMesh> pMesh = new CMesh;

    vector<Vtx> vecVtx;
    Vtx v;

    for (UINT Row = 0; Row < m_FaceZ + 1; ++Row)
    {
        for (UINT Col = 0; Col < m_FaceX + 1; ++Col)
        {
            v.vPos = Vec3(static_cast<float>(Col), 0.f, static_cast<float>(Row));
            v.vUV = Vec2(static_cast<float>(Col), static_cast<float>(m_FaceZ) - Row);

            v.vTangent = Vec3(1.f, 0.f, 0.f);
            v.vNormal = Vec3(0.f, 1.f, 0.f);
            v.vBinormal = Vec3(0.f, 0.f, -1.f);

            vecVtx.push_back(v);
        }
    }

    vector<UINT> vecIdx;
    for (UINT Row = 0; Row < m_FaceZ; ++Row)
    {
        for (UINT Col = 0; Col < m_FaceX; ++Col)
        {
            // 0
            // | \
            // 2--1
            vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
            vecIdx.push_back((m_FaceX + 1) * (Row) + Col + 1);
            vecIdx.push_back((m_FaceX + 1) * (Row) + Col);

            // 1--2
            //  \ |
            //    0
            vecIdx.push_back((m_FaceX + 1) * (Row) + Col + 1);
            vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
            vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col + 1);
        }
    }

    pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(),
                  static_cast<UINT>(vecIdx.size()));
    SetMesh(pMesh);

    // Mesh 를 변경하면, 재질을 재설정해주어야 함
    CreateMaterial();
}

void CLandScape::CreateMaterial()
{
    if (GetMaterial(0).Get())
        return;

    Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
    if (nullptr != pMtrl)
    {
        SetMaterial(pMtrl, 0);
        return;
    }

    Ptr<CGraphicShader> pShader = new CGraphicShader;

    pShader->CreateVertexShader(L"Shader\\landscape.fx", "VS_LandScape");
    pShader->CreateHullShader(L"Shader\\landscape.fx", "HS_LandScape");
    pShader->CreateDomainShader(L"Shader\\landscape.fx", "DS_LandScape");
    pShader->CreatePixelShader(L"Shader\\landscape.fx", "PS_LandScape");

    pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    pShader->SetRSState(RS_TYPE::CULL_BACK);
    pShader->SetDSState(DS_TYPE::LESS);
    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

    pShader->AddScalarParam("TessLevel", INT_2);

    CAssetMgr::GetInst()->AddAsset<CGraphicShader>(L"LandScapeShader", pShader);

    pMtrl = new CMaterial;
    pMtrl->SetName(L"LandScapeMtrl");
    pMtrl->SetShader(pShader);
    CAssetMgr::GetInst()->AddAsset<CMaterial>(L"LandScapeMtrl", pMtrl);

    // Material 세팅
    SetMaterial(pMtrl, 0);
}

void CLandScape::CreateComputeShader()
{
    m_HeightMapCS = new CHeightMapCS;

    m_RaycastCS = new CRaycastCS;

    m_WeightMapCS = new CWeightMapCS;

    // 가중치 WeightMap 용 StructuredBuffer
    m_WeightMap = new CStructuredBuffer;
    m_WeightWidth = 1024;
    m_WeightHeight = 1024;
    m_WeightMap->Create(sizeof(tWeight8), m_WeightWidth * m_WeightHeight, SRV_UAV, true,
                        nullptr);
}


void CLandScape::CreateHeightMap(UINT _Width, UINT _Height)
{
    if (nullptr != m_HeightMap)
    {
        CAssetMgr::GetInst()->DeleteAsset(TEXTURE, L"HeightMap");
    }

    m_HeightMap = CAssetMgr::GetInst()->CreateTexture(L"HeightMap"
                                                      , _Width, _Height, DXGI_FORMAT_R32_FLOAT
                                                      , D3D11_BIND_SHADER_RESOURCE |
                                                      D3D11_BIND_UNORDERED_ACCESS);
}

void CLandScape::SetColorTexture(Ptr<CTexture> _ArrTex)
{
    m_ColorTex = _ArrTex;

    if (m_ColorTex->GetMetaData().mipLevels <= 1)
        m_ColorTex->GenerateMip(5);
}

void CLandScape::SetNormalTexture(Ptr<CTexture> _ArrTex)
{
    m_NormalTex = _ArrTex;

    if (m_NormalTex->GetMetaData().mipLevels <= 1)
        m_NormalTex->GenerateMip(5);
}

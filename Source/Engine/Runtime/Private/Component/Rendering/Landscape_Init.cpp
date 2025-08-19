#include "pch.h"
#include "System/Public/Manager/AssetManager.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "Runtime/Public/Component/Rendering/Landscape.h"

void FLandscape::Init()
{
// 	CreateMesh();
// 	CreateMaterial();
// 	CreateComputeShader();
//
// 	Ptr<CTexture> GlowSceneTexture = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\brush\\TX_GlowScene_2.png");
// 	Ptr<CTexture> HitFlashTexture = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\brush\\TX_HitFlash_0.png");
// 	Ptr<CTexture> HitFlash02Texture = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\brush\\TX_HitFlash02.png");
// 	Ptr<CTexture> TwirlTexture = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\brush\\TX_Twirl02.png");
// 	Ptr<CTexture> FlareTexture = FAssetManager::GetInst()->Load<CTexture>(L"Texture\\brush\\FX_Flare.png");
//
// 	// BrushTexture 추가
// 	AddBrushTexture(GlowSceneTexture);
// 	AddBrushTexture(HitFlashTexture);
// 	AddBrushTexture(HitFlash02Texture);
// 	AddBrushTexture(TwirlTexture);
// 	AddBrushTexture(FlareTexture);
//
// 	// Raycasting 결과를 받는 용도의 구조화버퍼
// 	m_RaycastOut = new CStructuredBuffer;
// 	m_RaycastOut->Create(sizeof(tRaycastOut), 1, SRV_UAV, true);
//
// 	// 충돌 연산을 주고받는 용도의 구조화 버퍼
// 	m_RayCollisionOut = new CStructuredBuffer;
}

void FLandscape::CreateMesh()
{
	Ptr<CMesh> pMesh = new CMesh;

	vector<Vtx> vecVtx;
	Vtx v;

	for (UINT Row = 0; Row < FaceZ + 1; ++Row)
	{
		for (UINT Col = 0; Col < FaceX + 1; ++Col)
		{
			v.vPos = Vec3(static_cast<float>(Col), 0.f, static_cast<float>(Row));
			v.vUV = Vec2(static_cast<float>(Col), static_cast<float>(FaceZ - Row));

			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);

			vecVtx.push_back(v);
		}
	}

	vector<UINT> vecIdx;
	for (UINT Row = 0; Row < FaceZ; ++Row)
	{
		for (UINT Col = 0; Col < FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1
			vecIdx.push_back((FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((FaceX + 1) * (Row) + Col + 1);
			vecIdx.push_back((FaceX + 1) * (Row) + Col);

			// 1--2
			//  \ |
			//    0
			vecIdx.push_back((FaceX + 1) * (Row) + Col + 1);
			vecIdx.push_back((FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((FaceX + 1) * (Row + 1) + Col + 1);
		}
	}

	pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(),
	              static_cast<UINT>(vecIdx.size()));
	SetMesh(pMesh);

	// Mesh 를 변경하면, 재질을 재설정해주어야 함
	CreateMaterial();
}

void FLandscape::CreateMaterial()
{
	if (GetMaterial(0).Get())
		return;

	Ptr<CMaterial> pMtrl = FAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	if (nullptr != pMtrl)
	{
		SetMaterial(pMtrl, 0);
		return;
	}

	Ptr<CGraphicShader> pShader = new CGraphicShader;

	pShader->CreateVertexShader(L"landscape_vs.cso", L"landscape.fx", L"VS_LandScape");
	pShader->CreateHullShader(L"landscape_hs.cso", L"landscape.fx", L"HS_LandScape");
	pShader->CreateDomainShader(L"landscape_ds.cso", L"landscape.fx", L"DS_LandScape");
	pShader->CreatePixelShader(L"landscape_ps.cso", L"landscape.fx", L"PS_LandScape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pShader->SetRSState(RS_TYPE::CULL_BACK);
	pShader->SetDSState(DS_TYPE::LESS);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

	pShader->AddScalarParam("TessLevel", INT_2);

	FAssetManager::GetInst()->AddAsset<CGraphicShader>(L"LandScapeShader", pShader);

	pMtrl = new CMaterial;
	pMtrl->SetName(L"LandScapeMtrl");
	pMtrl->SetShader(pShader);
	FAssetManager::GetInst()->AddAsset<CMaterial>(L"LandScapeMtrl", pMtrl);

	// Material 세팅
	SetMaterial(pMtrl, 0);
}

// void FLandscape::CreateComputeShader()
// {
// 	m_HeightMapCS = new CHeightMapCS;
//
// 	m_RaycastCS = new CRaycastCS;
//
// 	m_WeightMapCS = new CWeightMapCS;
//
// 	// 가중치 WeightMap 용 StructuredBuffer
// 	m_WeightMap = new CStructuredBuffer;
// 	m_WeightWidth = 1024;
// 	m_WeightHeight = 1024;
// 	m_WeightMap->Create(sizeof(tWeight8), m_WeightWidth * m_WeightHeight,
// 	                    SRV_UAV, true, nullptr);
// }


void FLandscape::CreateHeightMap(UINT InWidth, UINT InHeight)
{
	if (nullptr != HeightMap)
	{
		FAssetManager::GetInst()->DeleteAsset(TEXTURE, L"HeightMap");
	}

	HeightMap = FAssetManager::GetInst()->CreateTexture(L"HeightMap"
	                                                  , InWidth, InHeight, DXGI_FORMAT_R32_FLOAT
	                                                  , D3D11_BIND_SHADER_RESOURCE |
	                                                  D3D11_BIND_UNORDERED_ACCESS);

	HeightMap->CaptureTextureCustom(CachedHeightData);
}

// void FLandscape::SetColorTexture(Ptr<CTexture> InArrTex)
// {
// 	m_ColorTex = InArrTex;
//
// 	if (m_ColorTex->GetMetaData().mipLevels <= 1)
// 		m_ColorTex->GenerateMip(5);
// }

// void FLandscape::SetNormalTexture(Ptr<CTexture> InArrTex)
// {
// 	m_NormalTex = InArrTex;
//
// 	if (m_NormalTex->GetMetaData().mipLevels <= 1)
// 		m_NormalTex->GenerateMip(5);
// }

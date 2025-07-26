#include "pch.h"
#include "Engine/System/Public/Manager/RenderManager.h"

#include "Engine/System/Public/Rendering/RenderPass/GeometryRenderPass.h"
#include "Engine/System/Public/Rendering/RenderPass/LightRenderPass.h"
#include "Engine/System/Public/Rendering/RenderPass/MergeRenderPass.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

void FRenderManager::Init()
{
	CreateResources();
	CreateMultiRenderTargets();
	CreateMaterials();
	CreateRenderPasses();
}

/**
 * @brief Render Manager Initialize에 필요한 Resource를 전부 로드하는 함수
 */
void FRenderManager::CreateResources()
{
	// Get Resolution
	// XXX(KHJ): 해당 부분 인자로 만들어서 가변 해상도 작업할 때 필요할 수 있을 듯
	Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
	UINT ResX = static_cast<UINT>(Resolution.x);
	UINT ResY = static_cast<UINT>(Resolution.y);

	auto BindFlag = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	/**************/
	/** Textures **/
	/**************/

	// SwapChain Texture
	// XXX(KHJ): SwapChain만 AssetLoad 하는 이유가 있나?
	RenderTargetTexture = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	DepthStencilTexture = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

	// Geometry Buffer Texture
	ColorTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"ColorTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R8G8B8A8_UNORM, BindFlag);
	NormalTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"NormalTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R32G32B32A32_FLOAT, BindFlag);
	WorldPositionTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"WorldPositionTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R32G32B32A32_FLOAT, BindFlag);
	EmissiveTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R8G8B8A8_UNORM, BindFlag);
	DataTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"DataTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R32G32B32A32_FLOAT, BindFlag);

	// Light Texture
	DiffuseTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R32G32B32A32_FLOAT, BindFlag);
	SpecularTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"SpecularTargetTexture", ResX, ResY,
		                                    DXGI_FORMAT_R32G32B32A32_FLOAT, BindFlag);

	// Minimap Texture
	MinimapTargetTexture =
		CAssetMgr::GetInst()->CreateTexture(L"MinimapTargetTexture", 256, 256,
		                                    DXGI_FORMAT_R8G8B8A8_UNORM, BindFlag);
	MinimapDepthStencilTexture =
		CAssetMgr::GetInst()->CreateTexture(L"MinimapDepthTexture", 256, 256,
		                                    DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	// PostProcess Texture
	PostProcessTexture = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessTex");

	/********************/
	/** Render Shaders **/
	/********************/

	// 여러 렌더 타겟(G-Buffer, 조명 결과 등)을 하나로 합치는 셰이더
	Ptr<CGraphicShader> MergeShader = new CGraphicShader;
	MergeShader->CreateVertexShader(L"merge_vs.cso", L"merge.fx", L"VS_Merge");
	MergeShader->CreatePixelShader(L"merge_ps.cso", L"merge.fx", L"PS_Merge");
	// 깊이 테스트/쓰기를 하지 않음
	MergeShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	// 알파 블렌딩 사용
	MergeShader->SetBSState(BS_TYPE::ALPHABLEND);
	CAssetMgr::GetInst()->AddAsset(L"MergeShader", MergeShader);

	// 방향성 광원(Directional Light) 계산을 위한 셰이더
	Ptr<CGraphicShader> DirectionalLightShader = new CGraphicShader;
	DirectionalLightShader->CreateVertexShader(L"directional_light_vs.cso", L"light.fx", L"VS_DirLight");
	DirectionalLightShader->CreatePixelShader(L"directional_light_ps.cso", L"light.fx", L"PS_DirLight");
	// 깊이 테스트/쓰기를 하지 않음
	DirectionalLightShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	// 덧셈 블렌딩(Additive Blending)을 사용하여 조명 결과를 누적
	DirectionalLightShader->SetBSState(BS_TYPE::ONE_ONE);
	CAssetMgr::GetInst()->AddAsset(L"DirectionalLightShader", DirectionalLightShader);

	// 점 광원(Point Light) 계산을 위한 셰이더
	Ptr<CGraphicShader> PointLightShader = new CGraphicShader;
	PointLightShader->CreateVertexShader(L"point_light_vs.cso", L"light.fx", L"VS_PointLight");
	PointLightShader->CreatePixelShader(L"point_light_ps.cso", L"light.fx", L"PS_PointLight");
	// 최적화를 위해 광원의 영향권 밖에 있는 픽셀을 컬링 (전면 컬링)
	PointLightShader->SetRSState(RS_TYPE::CULL_FRONT);
	// 깊이 테스트/쓰기를 하지 않음
	PointLightShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	// 덧셈 블렌딩 사용
	PointLightShader->SetBSState(BS_TYPE::ONE_ONE);
	CAssetMgr::GetInst()->AddAsset(L"PointLightShader", PointLightShader);

	/*******************/
	/** Debug Shaders **/
	/*******************/

	// Debug Shape Shader
	Ptr<CGraphicShader> DebugShapeShader = new CGraphicShader;
	// 버텍스/픽셀 셰이더 로드 및 생성
	DebugShapeShader->CreateVertexShader(L"debug_shape_vs.cso", L"debug.fx", L"VS_DebugShape");
	DebugShapeShader->CreatePixelShader(L"debug_shape_ps.cso", L"debug.fx", L"PS_DebugShape");
	// 래스터라이저 상태 설정 (컬링 없음)
	DebugShapeShader->SetRSState(RS_TYPE::CULL_NONE);
	// 기본 토폴로지를 라인 스트립으로 설정 (사각형, 원 등을 그리기 위함)
	DebugShapeShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	// 에셋 매니저에 셰이더 추가
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeShader", DebugShapeShader);

	// 구 형태의 디버그 도형을 위한 셰이더 생성
	Ptr<CGraphicShader> DebugShapeSphereShader = new CGraphicShader;
	DebugShapeSphereShader->CreateVertexShader(L"debug_shape_sphere_vs.cso", L"debug.fx", L"VS_DebugShapeSphere");
	DebugShapeSphereShader->CreatePixelShader(L"debug_shape_sphere_ps.cso", L"debug.fx", L"PS_DebugShapeSphere");
	DebugShapeSphereShader->SetRSState(RS_TYPE::CULL_NONE);
	// 반투명 효과를 위해 알파 블렌딩 설정
	DebugShapeSphereShader->SetBSState(BS_TYPE::ALPHABLEND);
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeSphereShader", DebugShapeSphereShader);

	// 선 형태의 디버그 도형을 위한 셰이더 생성 (지오메트리 셰이더 사용)
	Ptr<CGraphicShader> DebugShapeLineShader = new CGraphicShader;
	DebugShapeLineShader->CreateVertexShader(L"debug_shape_line_vs.cso", L"debug.fx", L"VS_DebugShapeLine");
	DebugShapeLineShader->CreateGeometryShader(L"debug_shape_line_gs.cso", L"debug.fx", L"GS_DebugShapeLine");
	DebugShapeLineShader->CreatePixelShader(L"debug_shape_line_ps.cso", L"debug.fx", L"PS_DebugShapeLine");
	DebugShapeLineShader->SetRSState(RS_TYPE::CULL_NONE);
	// 입력 토폴로지를 점 목록으로 설정 (지오메트리 셰이더에서 선으로 확장)
	DebugShapeLineShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeLineShader", DebugShapeLineShader);

	Ptr<CGraphicShader> DebugSkeletonShader = new CGraphicShader;
	DebugSkeletonShader->CreateVertexShader(L"debug_skeleton_vs.cso", L"debug.fx", L"VS_DebugSkeleton");
	DebugSkeletonShader->CreateGeometryShader(L"debug_skeleton_gs.cso", L"debug.fx", L"GS_DebugSkeleton");
	DebugSkeletonShader->CreatePixelShader(L"debug_skeleton_ps.cso", L"debug.fx", L"PS_DebugSkeleton");
	DebugSkeletonShader->SetRSState(RS_TYPE::CULL_NONE);
	DebugSkeletonShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CAssetMgr::GetInst()->AddAsset(L"DebugSkeletonShader", DebugSkeletonShader);
}

/**
 * @brief Multi Render Target (MRT) 를 생성하는 함수
 * MRT를 만드는 이유는 동시에 여러 Texture에 Pixel Data를 쓸 수 있도록 하기 위해서이다
 */
void FRenderManager::CreateMultiRenderTargets()
{
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)] = new FMultiRenderTarget();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)] = new FMultiRenderTarget();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::LIGHT)] = new FMultiRenderTarget();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::MINIMAP)] = new FMultiRenderTarget();

	/***************/
	/** SwapChain **/
	/***************/

	auto* SwapChainMRT = MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)];
	SwapChainMRT->SetName(L"SwapChainMRT");

	vector<Ptr<CTexture>> SwapChainTextures{RenderTargetTexture};

	SwapChainMRT->Init(SwapChainTextures, DepthStencilTexture);

	/**************/
	/** Deferred **/
	/**************/

	auto* DeferredMRT = MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)];
	DeferredMRT->SetName(L"DeferredMRT");

	vector<Ptr<CTexture>> DeferredTextures{
		ColorTargetTexture, NormalTargetTexture, WorldPositionTargetTexture,
		EmissiveTargetTexture, DataTargetTexture
	};

	// SwapChain과 동일한 Depth Stencil Texture 사용
	DeferredMRT->Init(DeferredTextures, DepthStencilTexture);

	/***********/
	/** Light **/
	/***********/

	auto* LightMRT = MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::LIGHT)];
	LightMRT->SetName(L"LightMRT");

	vector<Ptr<CTexture>> LightTextures{DiffuseTargetTexture, SpecularTargetTexture};

	// Depth Stencil Texture 필요하지 않음
	LightMRT->Init(LightTextures, nullptr);

	// Blending 처리를 위한 Clear All
	LightMRT->SetClearColorAll({0, 0, 0, 0});

	/*************/
	/** Minimap **/
	/*************/

	auto* MinimapMRT = MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::MINIMAP)];
	MinimapMRT->SetName(L"MinimapMRT");

	vector<Ptr<CTexture>> MinimapTextures{MinimapTargetTexture};

	MinimapMRT->Init(MinimapTextures, MinimapDepthStencilTexture);

	// 미니맵 배경을 투명하게 하기 위해 알파값을 0으로 클리어
	MinimapMRT->SetClearColor(0, {0, 0, 0, 0});
}

void FRenderManager::CreateMaterials()
{
	/**********************/
	/** Render Materials **/
	/**********************/

	Ptr<CGraphicShader> MergeShader
		= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"MergeShader");
	MergeMaterial = new CMaterial(true);
	MergeMaterial->SetName(L"MergeMaterial");
	MergeMaterial->SetShader(MergeShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(MergeMaterial->GetName(), MergeMaterial);

	Ptr<CGraphicShader> DirectionalLightShader
		= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"DirectionalLightShader");
	DirectionalLightMaterial = new CMaterial(true);
	DirectionalLightMaterial->SetName(L"DirectionalLightMaterial");
	DirectionalLightMaterial->SetShader(DirectionalLightShader);
	// 조명 계산에 필요한 G-Buffer 텍스처(Position, Normal)를 미리 설정
	DirectionalLightMaterial->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"WorldPositionTargetTexture"));
	DirectionalLightMaterial->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTexture"));
	CAssetMgr::GetInst()->AddAsset<CMaterial>(DirectionalLightMaterial->GetName(), DirectionalLightMaterial);

	Ptr<CGraphicShader> PointLightShader
		= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"PointLightShader");
	PointLightMaterial = new CMaterial(true);
	PointLightMaterial->SetName(L"PointLightMaterial");
	PointLightMaterial->SetShader(PointLightShader);
	// 조명 계산에 필요한 G-Buffer 텍스처(Position, Normal)를 미리 설정
	PointLightMaterial->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"WorldPositionTargetTexture"));
	PointLightMaterial->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTexture"));
	CAssetMgr::GetInst()->AddAsset<CMaterial>(PointLightMaterial->GetName(), PointLightMaterial);

	/*********************/
	/** Debug Materials **/
	/*********************/

	Ptr<CGraphicShader> DebugShapeShader
		= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"DebugShapeShader");
	DebugShapeMaterial = new CMaterial(true);
	DebugShapeMaterial->SetName(L"DebugShapeMaterial");
	DebugShapeMaterial->SetShader(DebugShapeShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(DebugShapeMaterial->GetName(), DebugShapeMaterial);

	Ptr<CGraphicShader> DebugShapeSphereShader
		= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"DebugShapeSphereShader");
	DebugShapeSphereMaterial = new CMaterial(true);
	DebugShapeSphereMaterial->SetName(L"DebugShapeSphereMaterial");
	DebugShapeSphereMaterial->SetShader(DebugShapeSphereShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(DebugShapeSphereMaterial->GetName(), DebugShapeSphereMaterial);

	Ptr<CGraphicShader> DebugShapeLineShader
	= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"DebugShapeLineShader");
	DebugShapeLineMaterial = new CMaterial(true);
	DebugShapeLineMaterial->SetName(L"DebugShapeLineMaterial");
	DebugShapeLineMaterial->SetShader(DebugShapeLineShader);
	CAssetMgr::GetInst()->AddAsset<CMaterial>(DebugShapeLineMaterial->GetName(), DebugShapeLineMaterial);

	Ptr<CGraphicShader> DebugSkeletonShader
	= CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"DebugSkeletonShader");
	DebugSkeletonMaterial = new CMaterial(true);
	DebugSkeletonMaterial->SetName(L"DebugSkeletonMaterial");
	DebugSkeletonMaterial->SetShader(DebugSkeletonShader);
	CAssetMgr::GetInst()->AddAsset(DebugSkeletonMaterial->GetName(), DebugSkeletonMaterial);

	// Debug Dummy Object
	DebugObject = new CGameObject;
	DebugObject->AddComponent(new CMeshRender);
}

void FRenderManager::CreateRenderPasses()
{
	GeometryPass = new FGeometryRenderPass;
	GeometryPass->SetName(L"GeometryPass");
	GeometryPass->SetTargetMRT(MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)]);

	Ptr<CMesh> RectMeshPtr = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	Ptr<CMesh> SphereMeshPtr = CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh");

	LightPass = new FLightRenderPass;
	LightPass->SetName(L"LightPass");
	LightPass->SetTargetMRT(MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::LIGHT)]);
	LightPass->AddMaterial(DirectionalLightMaterial);
	LightPass->AddMaterial(PointLightMaterial);
	LightPass->AddMesh(RectMeshPtr);
	LightPass->AddMesh(SphereMeshPtr);

	MergePass = new FMergeRenderPass;
	MergePass->SetName(L"MergePass");
	MergePass->SetTargetMRT(MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]);
	MergePass->AddMaterial(MergeMaterial);
	MergePass->AddMesh(RectMeshPtr);
}

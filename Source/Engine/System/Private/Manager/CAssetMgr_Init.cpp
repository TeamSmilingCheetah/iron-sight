#include "pch.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"
#include "System/Public/Rendering/Shader/CParticleTickCS.h"

void CAssetMgr::Init()
{
	CreateEngineMesh();
	CreateEngineTexture();
	CreateEngineGraphicShader();
	CreateEngineComputeShader();
	CreateEngineMaterial();
	CreateEngineSprite();
}

void CAssetMgr::CreateEngineTexture()
{
	// NoiseTexture
	Load<CTexture>(L"NoiseTex", L"Texture//noise//noise_03.jpg");

	// PostPorcessTexture
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	CreateTexture(L"PostProcessTex", static_cast<UINT>(vResolution.x), static_cast<UINT>(vResolution.y)
				  , DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	Ptr<CTexture> pEffectTarget = CreateTexture(
		L"EffectTargetTex"
		, static_cast<UINT>(vResolution.x), static_cast<UINT>(vResolution.y)
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	Ptr<CTexture> pEffectDepth = CreateTexture(
		L"EffectDepthStencilTex"
		, static_cast<UINT>(vResolution.x), static_cast<UINT>(vResolution.y)
		, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	Ptr<CTexture> pEffectBlurTarget = CreateTexture(
		L"EffectBlurTargetTex"
		, static_cast<UINT>(vResolution.x), static_cast<UINT>(vResolution.y)
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	// Editor Render Target (애니메이션 등 단일 애셋 확인용)
	// TODO : 필요에 따라 해상도 조절
	Ptr<CTexture> pEditorRenderTarget = CreateTexture(
		L"EditorRenderTargetTex"
		, static_cast<UINT>(vResolution.x), static_cast<UINT>(vResolution.y)
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

}

void CAssetMgr::CreateEngineGraphicShader()
{
	Ptr<CGraphicShader> pShader = nullptr;

	// ===========================
	// Std2DShader : 표준 2D 쉐이더
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std2d.cso",L"std2d.fx", L"VS_Std2D");
	pShader->CreatePixelShader(L"std2d.cso", L"std2d.fx", L"PS_Std2D");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	//pShader->SetDomain(SHADER_DOMAIN::DOMAIN_EFFECT);

	pShader->AddScalarParam("Test Parameter", VEC2_2, true);
	pShader->AddTexParam("Output Texture", TEX_0);

	AddAsset(L"Std2DShader", pShader);


	// ================================================
	// Std2DAlphaBlendShader : 표준 알파블렌드 2D 쉐이더
	// ================================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std2d_vs.cso", L"std2d.fx", L"VS_Std2D");
	pShader->CreatePixelShader(L"std2d_ps.cso", L"std2d.fx", L"PS_Std2D_AlphaBlend");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	AddAsset(L"Std2DAlphaBlendShader", pShader);


	// ===========================
	// Std3DShader : 표준 3D 쉐이더
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std3d_vs.cso", L"std3d.fx", L"VS_Std3D");
	pShader->CreatePixelShader(L"std3d_ps.cso", L"std3d.fx", L"PS_Std3D");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	//pShader->SetRSState(RS_TYPE::WIRE_FRAME);

	pShader->AddScalarParam("Shading Type", INT_0);

	AddAsset(L"Std3DShader", pShader);

	// ============================================
	// Std3D_DeferredShader : 표준 3D Deferred 쉐이더
	// ============================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std3d_deferred_vs.cso", L"std3d_deferred.fx", L"VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"std3d_deferred_ps.cso", L"std3d_deferred.fx", L"PS_Std3D_Deferred");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->AddTexParam("Base Color", TEX_0);
	pShader->AddTexParam("Normal", TEX_1);
	AddAsset(L"Std3D_DeferredShader", pShader);


	// =================================
	// Std2D_PaperBurn : PaperBurn 쉐이더
	// =================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std2d_paper_burn_vs.cso", L"std2d.fx", L"VS_Std2D");
	pShader->CreatePixelShader(L"std2d_paper_burn_ps.cso", L"std2d.fx", L"PS_Std2D_PaperBurn");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddAsset(L"Std2DPaperBurnShader", pShader);

	// ========================
	// UIShader : UI 전용 셰이더
	// ========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"ui_vs.cso", L"ui.fx", L"VS_UI");
	pShader->CreatePixelShader(L"ui_ps.cso", L"ui.fx", L"PS_UI");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	AddAsset(L"UIShader", pShader);

	// ========================
	// UICardinalShader : UI Cardinal 전용 셰이더
	// ========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"ui_cardinal_vs.cso", L"ui.fx", L"VS_UI_Cardinal");
	pShader->CreatePixelShader(L"ui_cardinal_ps.cso", L"ui.fx", L"PS_UI");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	AddAsset(L"UICardinalShader", pShader);

	// =============================
	// UIHPShader : UI HP 전용 셰이더
	// =============================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"ui_hp_vs.cso", L"ui.fx", L"VS_UI");
	pShader->CreatePixelShader(L"ui_hp_ps.cso", L"ui.fx", L"PS_UI_HP");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	AddAsset(L"UIHPShader", pShader);

	// =============================
	// UIHPShader : UI HP 전용 셰이더
	// =============================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"ui_item_use_vs.cso", L"ui.fx", L"VS_UI");
	pShader->CreatePixelShader(L"ui_item_use_ps.cso", L"ui.fx", L"PS_UI_ItemUse");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	AddAsset(L"UIItemUseShader", pShader);

	// =============================
	// UICrosshair : UI 크로스헤어 전용 셰이더
	// =============================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"ui_crosshair_vs.cso", L"ui.fx", L"VS_UI");
	pShader->CreatePixelShader(L"ui_crosshair_ps.cso", L"ui.fx", L"PS_UI_Crosshair");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	AddAsset(L"UICrosshair", pShader);


	// ==================================
	// TileMapShader : 타일맵 전용 쉐이더
	// ==================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"tilemap_vs.cso", L"tilemap.fx", L"VS_TileMap");
	pShader->CreatePixelShader(L"tilemap_vs.cso", L"tilemap.fx", L"PS_TileMap");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::LESS);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	AddAsset(L"TileMapShader", pShader);

	// ===========
	// PostProcess
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"postprocess_vs.cso", L"postprocess.fx", L"VS_Post");
	pShader->CreatePixelShader(L"postprocess_ps.cso", L"postprocess.fx", L"PS_Post");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	AddAsset(L"PostProcessShader", pShader);

	// ===========
	// TargetObjectPostProcess
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"targetpost_vs.cso", L"targetpost.fx", L"VS_TargetPost");
	pShader->CreatePixelShader(L"targetpost_ps.cso", L"targetpost.fx", L"PS_TargetPost");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	AddAsset(L"TargetPostShader", pShader);

	// ===========
	// Distortion
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"postprocess_distortion_vs.cso", L"postprocess.fx", L"VS_Distortion");
	pShader->CreatePixelShader(L"postprocess_distortion_ps.cso", L"postprocess.fx", L"PS_Distortion");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	AddAsset(L"DistortionShader", pShader);

	// ======
	// Vortex
	// ======
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"postprocess_vortex_vs.cso", L"postprocess.fx", L"VS_Vortex");
	pShader->CreatePixelShader(L"postprocess_vortex_ps.cso", L"postprocess.fx", L"PS_Vortex");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	AddAsset(L"VortexShader", pShader);

	// ==============
	// ParticleShader
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"particle_vs.cso", L"particle.fx", L"VS_Particle");
	pShader->CreateGeometryShader(L"particle_gs.cso", L"particle.fx", L"GS_Particle");
	pShader->CreatePixelShader(L"particle_ps.cso", L"particle.fx", L"PS_Particle");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_PARTICLE);
	AddAsset(L"ParticleShader", pShader);

	// ============
	// EffectShader
	// ============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"std2d_vs.cso", L"std2d.fx", L"VS_Effect");
	pShader->CreatePixelShader(L"std2d_ps.cso", L"std2d.fx", L"PS_Effect");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetDSState(DS_TYPE::LESS);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_EFFECT);
	AddAsset(L"EffectShader", pShader);

	// ==========
	// BlurShader
	// ==========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"postprocess_blur_vs.cso", L"postprocess.fx", L"VS_Blur");
	pShader->CreatePixelShader(L"postprocess_blur_ps.cso", L"postprocess.fx", L"PS_Blur");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"BlurShader", pShader);

	// ===========
	// EffectMerge
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"postprocess_effect_merge_vs.cso", L"postprocess.fx", L"VS_EffectMerge");
	pShader->CreatePixelShader(L"postprocess_effect_merge_ps.cso", L"postprocess.fx", L"PS_EffectMerge");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"EffectMergeShader", pShader);


	// ========
	// TessTest
	// ========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"tess_vs.cso", L"tess.fx", L"VS_Tess");
	pShader->CreateHullShader(L"tess_hs.cso", L"tess.fx", L"HS_Tess");
	pShader->CreateDomainShader(L"tess_ds.cso", L"tess.fx", L"DS_Tess");
	pShader->CreatePixelShader(L"tess_ps.cso", L"tess.fx", L"PS_Tess");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pShader->SetRSState(RS_TYPE::WIRE_FRAME);
	pShader->SetDSState(DS_TYPE::LESS);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->AddScalarParam("Tess Level", INT_0, true);
	pShader->AddScalarParam("Tess Level", FLOAT_0, true);

	AddAsset(L"TessShader", pShader);
}

void CAssetMgr::CreateEngineComputeShader()
{
	AddAsset<CComputeShader>(L"ParticleTickCS", new CParticleTickCS);
}

void CAssetMgr::CreateEngineMaterial()
{
	Ptr<CMaterial> pMtrl = nullptr;

	// Std2DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std2DAlphaBlendMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DAlphaBlendMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DAlphaBlendShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std3DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std3DMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std3DShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std3D_DeferredMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std3D_DeferredMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std3D_DeferredShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std2DPaperBurnMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DPaperBurnMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DPaperBurnShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// UIMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"UIMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"UIShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// UICardinalMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"UICardinalMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"UICardinalShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// UIHPMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"UIHPMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"UIHPShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// UIHPMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"UIItemUseMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"UIItemUseShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// UICrosshairMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"UICrosshairMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"UICrosshair"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// TileMapMaterial
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"TileMapMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TileMapShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// PostProcessMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"PostProcessMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"PostProcessShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// TargetPostProcessMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"TargetPostProcessMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TargetPostShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// DistortionMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DistortionMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DistortionShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"NoiseTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// VortexMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"VortexMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"VortexShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);


	// ParticleMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"ParticleMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"ParticleShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// EffectMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"EffectShader"));
	AddAsset(L"EffectMtrl", pMtrl);

	// BlurMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"BlurShader"));
	AddAsset(L"BlurMtrl", pMtrl);

	// EffectMergeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"EffectMergeShader"));
	AddAsset(L"EffectMergeMtrl", pMtrl);

	// TessMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TessShader"));
	AddAsset(L"TessMtrl", pMtrl);
}

void CAssetMgr::CreateEngineSprite()
{
	//Ptr<CTexture> pLinkTex = Load<CTexture>(L"LinkAtlas", L"Texture\\link.png");

	//Ptr<CFlipbook> pFlipbook = new CFlipbook(true);
	//Ptr<CSprite> pSprite = nullptr;

	//for (int i = 0; i < 10; ++i)
	//{
	//	pSprite = new CSprite(true);
	//	pSprite->SetAtlasTexture(pLinkTex);
	//	pSprite->SetLeftTop(Vec2(i * 120.f, 520.f));
	//	pSprite->SetSlice(Vec2(120.f, 130.f));
	//	pSprite->SetBackground(Vec2(200.f, 200.f));

	//	if (i == 3)
	//		pSprite->SetOffset(Vec2(10.f, 0.f));

	//	wchar_t szKey[255] = {};
	//	swprintf_s(szKey, 255, L"Sprite\\LinkWalkDown_%d.sprite", i);

	//	// Sprite 등록
	//	AddAsset(szKey, pSprite);

	//	// flipbook 에 Sprite 추가
	//	pFlipbook->AddSprite(pSprite);
	//
	//	// Sprite 저장
	//	pSprite->Save(CPathMgr::GetContentPath() + pSprite->GetKey());
	//}

	//// flipbook 등록
	//AddAsset(L"Flipbook\\LinkWalkDown.flip", pFlipbook);

	//// Flipbook 저장
	//pFlipbook->Save(CPathMgr::GetContentPath() + pFlipbook->GetKey());
}

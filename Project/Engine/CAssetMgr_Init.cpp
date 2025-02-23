#include "pch.h"
#include "CAssetMgr.h"

#include "CDevice.h"

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
    CreateTexture(L"PostProcessTex", vResolution.x, vResolution.y
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
}

void CAssetMgr::CreateEngineGraphicShader()
{
    Ptr<CGraphicShader> pShader = nullptr;

    // ===========================
    // Std2DShader : ǥ�� 2D ���̴�
    // ===========================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
    pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D");

    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
    //pShader->SetDomain(SHADER_DOMAIN::DOMAIN_EFFECT);

    pShader->AddScalarParam("Test Parameter", VEC2_2, true);
    pShader->AddTexParam("Output Texture", TEX_0);

    GetInst()->AddAsset(L"Std2DShader", pShader);


    // ================================================
    // Std2DAlphaBlendShader : ǥ�� ���ĺ����� 2D ���̴�
    // ================================================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
    pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D_AlphaBlend");
    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetBSState(BS_TYPE::ALPHABLEND);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
    GetInst()->AddAsset(L"Std2DAlphaBlendShader", pShader);


    // ===========================
    // Std3DShader : ǥ�� 3D ���̴�
    // ===========================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Std3D");
    pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Std3D");
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
    //pShader->SetRSState(RS_TYPE::WIRE_FRAME);

    pShader->AddScalarParam("Shading Type", INT_0);

    GetInst()->AddAsset(L"Std3DShader", pShader);

    // ============================================
    // Std3D_DeferredShader : ǥ�� 3D Deferred ���̴�
    // ============================================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
    pShader->CreatePixelShader(L"Shader\\std3d_deferred.fx", "PS_Std3D_Deferred");
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
    GetInst()->AddAsset(L"Std3D_DeferredShader", pShader);


    // =================================
    // Std2D_PaperBurn : PaperBurn ���̴�
    // =================================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
    pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D_PaperBurn");

    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

    GetInst()->AddAsset(L"Std2DPaperBurnShader", pShader);


    // ==================================
    // TileMapShader : Ÿ�ϸ� ���� ���̴�
    // ==================================
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\tilemap.fx", "VS_TileMap");
    pShader->CreatePixelShader(L"Shader\\tilemap.fx", "PS_TileMap");

    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDSState(DS_TYPE::LESS);

    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
    GetInst()->AddAsset(L"TileMapShader", pShader);

    // ===========
    // PostProcess
    // ===========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Post");
    pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Post");

    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetRSState(RS_TYPE::CULL_NONE);

    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

    GetInst()->AddAsset(L"PostProcessShader", pShader);


    // ===========
    // Distortion
    // ===========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Distortion");
    pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Distortion");

    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetRSState(RS_TYPE::CULL_NONE);

    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

    GetInst()->AddAsset(L"DistortionShader", pShader);

    // ===========
    // Vortex
    // ===========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Vortex");
    pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Vortex");

    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetRSState(RS_TYPE::CULL_NONE);

    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

    GetInst()->AddAsset(L"VortexShader", pShader);


    // ==============
    // ParticleShader
    // ==============
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\particle.fx", "VS_Particle");
    pShader->CreateGeometryShader(L"Shader\\particle.fx", "GS_Particle");
    pShader->CreatePixelShader(L"Shader\\particle.fx", "PS_Particle");
    pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetBSState(BS_TYPE::ALPHABLEND);
    pShader->SetDSState(DS_TYPE::NO_WRITE);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_PARTICLE);
    GetInst()->AddAsset(L"ParticleShader", pShader);

    // ============
    // EffectShader
    // ============
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Effect");
    pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Effect");
    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetDSState(DS_TYPE::LESS);
    pShader->SetBSState(BS_TYPE::ALPHABLEND);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_EFFECT);
    AddAsset(L"EffectShader", pShader);

    // ==========
    // BlurShader
    // ==========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_Blur");
    pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_Blur");
    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetBSState(BS_TYPE::ALPHABLEND);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
    AddAsset(L"BlurShader", pShader);

    // ===========
    // EffectMerge
    // ===========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_EffectMerge");
    pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_EffectMerge");
    pShader->SetRSState(RS_TYPE::CULL_NONE);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetBSState(BS_TYPE::ALPHABLEND);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
    AddAsset(L"EffectMergeShader", pShader);


    // ========
    // TessTest
    // ========
    pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"shader\\tess.fx", "VS_Tess");
    pShader->CreateHullShader(L"shader\\tess.fx", "HS_Tess");
    pShader->CreateDomainShader(L"shader\\tess.fx", "DS_Tess");
    pShader->CreatePixelShader(L"shader\\tess.fx", "PS_Tess");
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
    pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    pShader->SetRSState(RS_TYPE::WIRE_FRAME);
    pShader->SetDSState(DS_TYPE::LESS);
    pShader->SetBSState(BS_TYPE::DEFAULT);
    pShader->AddScalarParam("Tess Level", INT_0, true);
    pShader->AddScalarParam("Tess Level", FLOAT_0, true);

    AddAsset(L"TessShader", pShader);
}

#include "CParticleTickCS.h"

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

    // EffectMergeMtrl
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

    //	// Sprite ���
    //	AddAsset(szKey, pSprite);

    //	// flipbook �� Sprite �߰�
    //	pFlipbook->AddSprite(pSprite);
    //			
    //	// Sprite ����
    //	pSprite->Save(CPathMgr::GetInst()->GetContentPath() + pSprite->GetKey());
    //}

    //// flipbook ���
    //AddAsset(L"Flipbook\\LinkWalkDown.flip", pFlipbook);

    //// Flipbook ����
    //pFlipbook->Save(CPathMgr::GetInst()->GetContentPath() + pFlipbook->GetKey());
}

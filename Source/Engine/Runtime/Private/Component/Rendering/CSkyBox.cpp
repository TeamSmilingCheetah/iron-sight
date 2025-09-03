#include "pch.h"
#include "Engine/Runtime/Public/Component/Rendering/CSkyBox.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/AssetManager.h"

#include "Engine/System/Public/Manager/CRenderMgr.h"

CSkyBox::CSkyBox()
    : FRenderComponent(COMPONENT_TYPE::SKYBOX)
    , m_Mode(SPHERE)
{
    SetMode(m_Mode);
    CreateMaterial();
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::FinalTick()
{
	// TEST(Ssio) : Environment
	CRenderMgr::GetInst()->RegisterEnvironment(this);
}

void CSkyBox::Render()
{
    Transform()->Binding();

    if (SPHERE == m_Mode && nullptr != m_EnvTex && !m_EnvTex->IsCubeMap())
    {
        GetMaterial(0)->SetTexParam(TEX_0, m_EnvTex);
    }

    else if (CUBE == m_Mode && nullptr != m_EnvTex && m_EnvTex->IsCubeMap())
    {
        GetMaterial(0)->SetTexParam(TEX_CUBE_0, m_EnvTex);
    }

    GetMaterial(0)->SetScalarParam(INT_0, static_cast<int>(m_Mode));
    GetMaterial(0)->Binding();

    GetMesh()->Render(0);
}


void CSkyBox::CreateMaterial()
{
    if (GetMaterial(0).Get())
        return;

    Ptr<CMaterial> pMtrl = FAssetManager::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl");
    if (nullptr != pMtrl)
    {
        SetMaterial(pMtrl, 0);
        return;
    }

    Ptr<CGraphicShader> pSkyBoxShader = new CGraphicShader;
    pSkyBoxShader->CreateVertexShader(L"skybox_vs.cso", L"skybox.fx", L"VS_SkyBox");
    pSkyBoxShader->CreatePixelShader(L"skybox_ps.cso", L"skybox.fx", L"PS_SkyBox");
    pSkyBoxShader->SetRSState(RS_TYPE::CULL_FRONT);
    pSkyBoxShader->SetDSState(DS_TYPE::LESS_EQUAL);
    pSkyBoxShader->SetBSState(BS_TYPE::DEFAULT);
    pSkyBoxShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
    FAssetManager::GetInst()->AddAsset<CGraphicShader>(L"SkyBoxShader", pSkyBoxShader);

    pMtrl = new CMaterial;
    pMtrl->SetName(L"SkyBoxMtrl");
    pMtrl->SetShader(pSkyBoxShader);
    FAssetManager::GetInst()->AddAsset<CMaterial>(L"SkyBoxMtrl", pMtrl);

    // Material 세팅
    SetMaterial(pMtrl, 0);
}

void CSkyBox::SetMode(SKYBOX_MODE _Mode)
{
    m_Mode = _Mode;
    if (CUBE == m_Mode)
    {
        SetMesh(FAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
    }
    else
    {
        SetMesh(FAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    }

	SetMaterial(FAssetManager::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"), 0);
}

void CSkyBox::SaveComponent(FILE* _File)
{
	SaveAssetRef(m_EnvTex, _File);
	fwrite(&m_Mode, sizeof(SKYBOX_MODE), 1, _File);
}

void CSkyBox::LoadComponent(FILE* _File)
{
	LoadAssetRef(m_EnvTex, _File);
	fread(&m_Mode, sizeof(SKYBOX_MODE), 1, _File);
	SetMode(m_Mode);
	Transform()->SetFrustumCheck(false);
}

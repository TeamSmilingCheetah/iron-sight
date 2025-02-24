#include "pch.h"
#include "CSkyBox.h"

#include "CAssetMgr.h"
#include "CTransform.h"
#include "CGraphicShader.h"

CSkyBox::CSkyBox()
    : CRenderComponent(COMPONENT_TYPE::SKYBOX)
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
}

void CSkyBox::Render()
{
    Transform()->Binding();

    if (SPHERE == m_Mode && nullptr != m_SkyBoxTex && !m_SkyBoxTex->IsCubeMap())
    {
        GetMaterial(0)->SetTexParam(TEX_0, m_SkyBoxTex);
    }

    else if (CUBE == m_Mode && nullptr != m_SkyBoxTex && m_SkyBoxTex->IsCubeMap())
    {
        GetMaterial(0)->SetTexParam(TEX_CUBE_0, m_SkyBoxTex);
    }

    GetMaterial(0)->SetScalarParam(INT_0, static_cast<int>(m_Mode));
    GetMaterial(0)->Binding();

    GetMesh()->Render(0);
}


void CSkyBox::CreateMaterial()
{
    if (GetMaterial(0).Get())
        return;

    Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl");
    if (nullptr != pMtrl)
    {
        SetMaterial(pMtrl, 0);
        return;
    }

    Ptr<CGraphicShader> pSkyBoxShader = new CGraphicShader;
    pSkyBoxShader->CreateVertexShader(L"Shader\\skybox.fx", "VS_SkyBox");
    pSkyBoxShader->CreatePixelShader(L"Shader\\skybox.fx", "PS_SkyBox");
    pSkyBoxShader->SetRSState(RS_TYPE::CULL_FRONT);
    pSkyBoxShader->SetDSState(DS_TYPE::LESS_EQUAL);
    pSkyBoxShader->SetBSState(BS_TYPE::DEFAULT);
    pSkyBoxShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
    CAssetMgr::GetInst()->AddAsset<CGraphicShader>(L"SkyBoxShader", pSkyBoxShader);

    pMtrl = new CMaterial;
    pMtrl->SetName(L"SkyBoxMtrl");
    pMtrl->SetShader(pSkyBoxShader);
    CAssetMgr::GetInst()->AddAsset<CMaterial>(L"SkyBoxMtrl", pMtrl);

    // Material 세팅
    SetMaterial(pMtrl, 0);
}

void CSkyBox::SetMode(SKYBOX_MODE _Mode)
{
    m_Mode = _Mode;
    if (CUBE == m_Mode)
    {
        SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
        SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"), 0);
    }
    else
    {
        SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
        SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"), 0);
    }
}

void CSkyBox::SaveComponent(FILE* _File)
{
}

void CSkyBox::LoadComponent(FILE* _File)
{
}

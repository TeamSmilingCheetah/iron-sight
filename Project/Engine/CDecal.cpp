#include "pch.h"
#include "CDecal.h"
#include "CRenderMgr.h"
#include "CCamera.h"
#include "CAssetMgr.h"
#include "CTransform.h"

CDecal::CDecal()
    : CRenderComponent(COMPONENT_TYPE::DECAL)
      , m_GlobalAlpha(1.f)
      , m_AsLight(false)
{
    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh"));

    CreateMaterial();
}

CDecal::~CDecal()
{
}

void CDecal::FinalTick()
{
    DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f)
                  , Transform()->GetWorldPos()
                  , Transform()->GetWorldScale()
                  , Transform()->GetRelativeRotation(), true, 0.f);
}

void CDecal::Render()
{
    /*m_GlobalAlpha -= 0.1f * EngineDT;
    if(m_GlobalAlpha < 0.f)
    {
        DestroyObject(GetOwner());
    }*/

    // 위치정보
    Transform()->Binding();

    // 재질 바인딩
    CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCamera();
    Matrix matInv = pMainCam->GetViewInvMat() * Transform()->GetWorldInvMat();
    GetMaterial(0)->SetScalarParam(MAT_0, matInv);
    GetMaterial(0)->SetScalarParam(FLOAT_0, m_GlobalAlpha);
    GetMaterial(0)->SetTexParam(TEX_1, m_DecalTex);
    GetMaterial(0)->SetScalarParam(INT_0, static_cast<int>(m_AsLight));
    GetMaterial(0)->Binding();

    // 렌더링
    GetMesh()->Render(0);

    // 텍스쳐 클리어
    CTexture::Clear(0);
    CTexture::Clear(1);
}

void CDecal::CreateMaterial()
{
    if (GetMaterial(0).Get())
        return;

    Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DecalMtrl");
    if (nullptr != pMtrl)
    {
        SetMaterial(pMtrl, 0);
        return;
    }

    Ptr<CGraphicShader> pShader = new CGraphicShader;
    pShader->CreateVertexShader(L"Shader\\decal.fx", "VS_Decal");
    pShader->CreatePixelShader(L"Shader\\decal.fx", "PS_Decal");
    pShader->SetRSState(RS_TYPE::CULL_FRONT);
    pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
    pShader->SetBSState(BS_TYPE::DECAL);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);
    CAssetMgr::GetInst()->AddAsset<CGraphicShader>(L"DecalShader", pShader);

    pMtrl = new CMaterial;
    pMtrl->SetName(L"DecalMtrl");
    pMtrl->SetShader(pShader);
    pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));

    CAssetMgr::GetInst()->AddAsset<CMaterial>(L"DecalMtrl", pMtrl);

    // Material 세팅
    SetMaterial(pMtrl, 0);
}

void CDecal::SaveComponent(FILE* _File)
{
}

void CDecal::LoadComponent(FILE* _File)
{
}

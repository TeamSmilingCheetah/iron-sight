#include "pch.h"
#include "CLight3D.h"

#include "CAssetMgr.h"

#include "CRenderMgr.h"
#include "CTransform.h"
#include "CCamera.h"

CLight3D::CLight3D()
    : CComponent(COMPONENT_TYPE::LIGHT3D)
      , m_LightIdx(-1)
{
}

CLight3D::~CLight3D()
{
}

void CLight3D::SetLightType(LIGHT_TYPE _Type)
{
    m_LightInfo.Type = _Type;

    if (LIGHT_TYPE::DIRECTIONAL == m_LightInfo.Type)
    {
        m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
        m_LightMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DirLightMtrl");
    }

    else if (LIGHT_TYPE::POINT == m_LightInfo.Type)
    {
        m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh");
        m_LightMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"PointLightMtrl");
    }

    else if (LIGHT_TYPE::SPOT == m_LightInfo.Type)
    {
        //m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"ConeMesh");
    }
}

void CLight3D::FinalTick()
{
    // 위치정보 갱신
    m_LightInfo.WorldPos = Transform()->GetWorldPos();
    m_LightInfo.Dir = Transform()->GetWorldDir(DIR_TYPE::FRONT);

    // RenderMgr 에 Light3D 등록	
    m_LightIdx = CRenderMgr::GetInst()->RegisterLight3D(this);

    // DebugRender
    if (LIGHT_TYPE::POINT == m_LightInfo.Type)
    {
        DrawDebugSphere(Vec4(0.f, 0.f, 1.f, 1.f), Transform()->GetWorldPos()
                        , m_LightInfo.Radius, true, 0.f);
    }
}

void CLight3D::Render()
{
    if (m_LightInfo.Type != LIGHT_TYPE::DIRECTIONAL)
    {
        // 볼륨메쉬 안에 물체가 존재하는지 체크할때 사용할 역행렬
        CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCamera();
        Matrix matInv = pMainCam->GetViewInvMat() * Transform()->GetWorldInvMat();
        m_LightMtrl->SetScalarParam(MAT_0, matInv);

        Transform()->Binding();
    }

    m_LightMtrl->SetScalarParam(INT_0, m_LightIdx);
    m_LightMtrl->Binding();

    m_VolumeMesh->Render(0);

    CTexture::Clear(0);
    CTexture::Clear(1);
}


void CLight3D::SetRadius(float _Radius)
{
    m_LightInfo.Radius = _Radius;
    Transform()->SetRelativeScale(_Radius * 2.f, _Radius * 2.f, _Radius * 2.f);
}


void CLight3D::SaveComponent(FILE* _File)
{
    fwrite(&m_LightInfo, sizeof(tLight3DInfo), 1, _File);
}

void CLight3D::LoadComponent(FILE* _File)
{
    fread(&m_LightInfo, sizeof(tLight3DInfo), 1, _File);
}

#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "components.h"
#include "CRenderMgr.h"
#include "CMRT.h"

#include "CFrustum.h"

CCamera::CCamera()
    : CComponent(COMPONENT_TYPE::CAMERA)
      , m_Frustum(nullptr)
      , m_ProjType(ORTHOGRAPHIC)
      , m_Far(10000.f)
      , m_AspectRatio(1.f)
      , m_LayerCheck(0)
      , m_Priority(-1)
      , m_FOV(XM_PI / 2.f)
      , m_Width(0)
      , m_Scale(1.f)
{
    m_Frustum = new CFrustum;
    m_Frustum->SetOwner(this);

    Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
    m_Width = static_cast<UINT>(vResolution.x);
    m_AspectRatio = vResolution.x / vResolution.y;
}

CCamera::CCamera(const CCamera& _Origin)
    : CComponent(_Origin)
      , m_Frustum(nullptr)
      , m_ProjType(_Origin.m_ProjType)
      , m_Far(_Origin.m_Far)
      , m_AspectRatio(_Origin.m_AspectRatio)
      , m_LayerCheck(_Origin.m_LayerCheck)
      , m_Priority(_Origin.m_Priority)
      , m_FOV(_Origin.m_FOV)
      , m_Width(_Origin.m_Width)
      , m_Scale(_Origin.m_Scale)
{
    m_Frustum = _Origin.m_Frustum->Clone();
    m_Frustum->SetOwner(this);
}

CCamera::~CCamera()
{
    if (nullptr != m_Frustum)
        delete m_Frustum;
}

void CCamera::Begin()
{
    // RenderMgr 에 등록
    CRenderMgr::GetInst()->RegisterCamera(this, m_Priority);
}

void CCamera::FinalTick()
{
    // ViewSpace
    // 1. 카메라가 있는곳이 원점
    // 2. 카메라가 바라보는 방향이 Z 축

    // View 행렬 계산
    Vec3 vWorldPos = Transform()->GetRelativePos();
    Matrix matTrans = XMMatrixTranslation(-vWorldPos.x, -vWorldPos.y, -vWorldPos.z);

    Matrix matRot = XMMatrixIdentity();

    Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
    Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
    Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

    matRot._11 = vR.x;
    matRot._12 = vU.x;
    matRot._13 = vF.x;
    matRot._21 = vR.y;
    matRot._22 = vU.y;
    matRot._23 = vF.y;
    matRot._31 = vR.z;
    matRot._32 = vU.z;
    matRot._33 = vF.z;

    m_matView = matTrans * matRot;
    m_matViewInv = XMMatrixInverse(nullptr, m_matView);


    // Proj 행렬 계산
    if (ORTHOGRAPHIC == m_ProjType)
        m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale,
                                           1.f, m_Far);
    else
        m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);

    m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

    // 마우스방향 Ray 계산
    CalcRay();

    // Frustum 생성
    m_Frustum->FinalTick();
}


void CCamera::SortObject()
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        if (m_LayerCheck & (1 << i))
        {
            CLayer* pLayer = pCurLevel->GetLayer(i);

            const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

            for (size_t j = 0; j < vecObjects.size(); ++j)
            {
                CRenderComponent* pRenderCom = vecObjects[j]->GetRenderComponent();

                // 분류 예외조건 검사
                if (nullptr == pRenderCom
                    || nullptr == pRenderCom->GetMesh()
                    || nullptr == pRenderCom->GetMaterial(0)
                    || nullptr == pRenderCom->GetMaterial(0)->GetShader())
                    continue;

                // FrustumCheck
                if (false == FrustumCheck(vecObjects[j]))
                {
                    continue;
                }

                SHADER_DOMAIN domain = pRenderCom->GetMaterial(0)->GetShader()->GetDomain();

                switch (domain)
                {
                case SHADER_DOMAIN::DOMAIN_DEFERRED:
                    m_vecDeferred.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_DECAL:
                    m_vecDecal.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_OPAQUE:
                    m_vecOpaque.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_MASK:
                    m_vecMask.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
                    m_vecTransparent.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_EFFECT:
                    m_vecEffect.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_PARTICLE:
                    m_vecParticle.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
                    m_vecPostprocess.push_back(vecObjects[j]);
                    break;
                default:
                    assert(nullptr);
                }
            }
        }
    }
}

void CCamera::render_deferred()
{
    for (size_t i = 0; i < m_vecDeferred.size(); ++i)
    {
        m_vecDeferred[i]->Render();
    }
}

void CCamera::render_decal()
{
    for (size_t i = 0; i < m_vecDecal.size(); ++i)
    {
        m_vecDecal[i]->Render();
    }
}

void CCamera::render_forward()
{
    // 불투명
    for (size_t i = 0; i < m_vecOpaque.size(); ++i)
    {
        m_vecOpaque[i]->Render();
    }

    // Mask
    for (size_t i = 0; i < m_vecMask.size(); ++i)
    {
        m_vecMask[i]->Render();
    }

    // 반투명
    for (size_t i = 0; i < m_vecTransparent.size(); ++i)
    {
        m_vecTransparent[i]->Render();
    }
}

void CCamera::render_effect()
{
    // 렌더타겟 변경
    Ptr<CTexture> pEffectTarget = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EffectTargetTex");
    Ptr<CTexture> pEffectDepth = CAssetMgr::GetInst()->FindAsset<
        CTexture>(L"EffectDepthStencilTex");

    // 클리어
    CONTEXT->ClearRenderTargetView(pEffectTarget->GetRTV().Get(), Vec4(0.f, 0.f, 0.f, 0.f));
    CONTEXT->ClearDepthStencilView(pEffectDepth->GetDSV().Get(),
                                   D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.f, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = pEffectTarget->GetWidth();
    viewport.Height = pEffectTarget->GetHeight();
    viewport.MaxDepth = 1.f;

    CONTEXT->RSSetViewports(1, &viewport);
    CONTEXT->OMSetRenderTargets(1, pEffectTarget->GetRTV().GetAddressOf(),
                                pEffectDepth->GetDSV().Get());

    // Effect
    for (size_t i = 0; i < m_vecEffect.size(); ++i)
    {
        m_vecEffect[i]->Render();
    }

    // BlurTarget 으로 변경
    Ptr<CTexture> pEffectBlurTarget = CAssetMgr::GetInst()->FindAsset<CTexture>(
        L"EffectBlurTargetTex");
    Ptr<CMaterial> pBlurMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlurMtrl");
    Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");

    CONTEXT->ClearRenderTargetView(pEffectBlurTarget->GetRTV().Get(), Vec4(0.f, 0.f, 0.f, 0.f));

    CONTEXT->RSSetViewports(1, &viewport);
    CONTEXT->OMSetRenderTargets(1, pEffectBlurTarget->GetRTV().GetAddressOf(), nullptr);

    pBlurMtrl->SetTexParam(TEX_0, pEffectTarget);
    pBlurMtrl->Binding();
    pRectMesh->Render_Particle(2);

    CTexture::Clear(0);


    // 원래 렌더타겟으로 변경
    Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
    Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
    Ptr<CMaterial> pEffectMergeMtrl = CAssetMgr::GetInst()->FindAsset<
        CMaterial>(L"EffectMergeMtrl");

    viewport.Width = pRTTex->GetWidth();
    viewport.Height = pRTTex->GetHeight();
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    CONTEXT->RSSetViewports(1, &viewport);
    CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

    pEffectMergeMtrl->SetTexParam(TEX_0, pEffectTarget);
    pEffectMergeMtrl->SetTexParam(TEX_1, pEffectBlurTarget);
    pEffectMergeMtrl->Binding();
    pRectMesh->Render(0);

    CTexture::Clear(0);
    CTexture::Clear(1);
}

void CCamera::render_particle()
{
    for (size_t i = 0; i < m_vecParticle.size(); ++i)
    {
        m_vecParticle[i]->Render();
    }
}

void CCamera::render_postprocess()
{
    for (size_t i = 0; i < m_vecPostprocess.size(); ++i)
    {
        CRenderMgr::GetInst()->CopyRenderTarget();
        m_vecPostprocess[i]->Render();
    }
}

void CCamera::render_clear()
{
    m_vecDeferred.clear();
    m_vecDecal.clear();
    m_vecOpaque.clear();
    m_vecMask.clear();
    m_vecTransparent.clear();
    m_vecEffect.clear();
    m_vecParticle.clear();
    m_vecPostprocess.clear();
}

void CCamera::LayerCheck(int _LayerIdx)
{
    if (m_LayerCheck & (1 << _LayerIdx))
    {
        m_LayerCheck &= ~(1 << _LayerIdx);
    }

    else
    {
        m_LayerCheck |= (1 << _LayerIdx);
    }
}

bool CCamera::FrustumCheck(CGameObject* _Object)
{
    if (false == _Object->Transform()->IsFrustumCheck())
        return true;

    DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f)
                    , _Object->Transform()->GetWorldPos()
                    , _Object->Transform()->GetFrustumRadius(), false, 0.f);

    Vec3 vWorldPos = _Object->Transform()->GetWorldPos();
    float Radius = _Object->Transform()->GetFrustumRadius();

    if (m_Frustum->FrustumCheckSphere(vWorldPos, Radius))
        return true;

    return false;
}

void CCamera::CalcRay()
{
    // ViewPort 정보
    CMRT* pSwapChainMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
    if (nullptr == pSwapChainMRT)
        return;

    // 메인 MRT 의 ViewPort 값을 가져온다.
    const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

    // 현재 마우스 좌표
    Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

    // 마우스를 향하는 직선은 카메라 위치를 지난다.
    m_Ray.vStart = Transform()->GetWorldPos();

    // View 공간 상에서 카메라에서 마우스 방향을 향하는 방향벡터를 구한다.
    //  - 마우스가 있는 좌표를 -1 ~ 1 사이의 정규화된 좌표로 바꾼다.
    //  - 투영행렬의 _11, _22 에 있는 값은 Near 평면상에서 Near 값을 가로 세로 길이로 나눈값
    //  - 실제 ViewSpace 상에서의 Near 평명상에서 마우스가 있는 지점을 향하는 위치를 구하기 위해서 비율을 나누어서 
    //  - 실제 Near 평면상에서 마우스가 향하는 위치를 좌표를 구함
    m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
    m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
    m_Ray.vDir.z = 1.f;

    // 방향 벡터에 ViewMatInv 를 적용, 월드상에서의 방향을 알아낸다.
    m_Ray.vDir = XMVector3TransformNormal(m_Ray.vDir, m_matViewInv);
    m_Ray.vDir.Normalize();
}

void CCamera::SaveComponent(FILE* _File)
{
    fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
    fwrite(&m_Far, sizeof(float), 1, _File);
    fwrite(&m_AspectRatio, sizeof(float), 1, _File);
    fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
    fwrite(&m_Priority, sizeof(int), 1, _File);
    fwrite(&m_FOV, sizeof(float), 1, _File);
    fwrite(&m_Width, sizeof(float), 1, _File);
    fwrite(&m_Scale, sizeof(float), 1, _File);
}

void CCamera::LoadComponent(FILE* _File)
{
    fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
    fread(&m_Far, sizeof(float), 1, _File);
    fread(&m_AspectRatio, sizeof(float), 1, _File);
    fread(&m_LayerCheck, sizeof(UINT), 1, _File);
    fread(&m_Priority, sizeof(int), 1, _File);
    fread(&m_FOV, sizeof(float), 1, _File);
    fread(&m_Width, sizeof(float), 1, _File);
    fread(&m_Scale, sizeof(float), 1, _File);
}

#include "pch.h"
#include "Runtime/Public/Component/Rendering/CDecal.h"
#include "Runtime/Public/Component/Camera/CCamera.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/RenderManager.h"
#include "System/Public/Manager/CTimeMgr.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	  , m_GlobalAlpha(1.f)
	  , m_AsLight(false)
	  , m_LifeTime(0.f)
	  , m_CurClipAccTime(0.f)
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

	m_CurClipAccTime += DT;

	// 수명이 0이라면 영구
	if (m_LifeTime == 0)
	{
		return;
	}

	if (m_LifeTime < m_CurClipAccTime)
	{
		DestroyObject(GetOwner());
	}


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
	CCamera* pMainCam = FRenderManager::GetInst()->GetMainCamera();
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
	pShader->CreateVertexShader(L"decal.cso", L"decal.fx", L"VS_Decal");
	pShader->CreatePixelShader(L"decal.cso", L"decal.fx", L"PS_Decal");
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
	fwrite(&m_GlobalAlpha, sizeof(float), 1, _File);
	fwrite(&m_AsLight, sizeof(bool), 1, _File);

	SaveAssetRef(m_DecalTex, _File);
}

void CDecal::LoadComponent(FILE* _File)
{
	fread(&m_GlobalAlpha, sizeof(float), 1, _File);
	fread(&m_AsLight, sizeof(bool), 1, _File);

	LoadAssetRef(m_DecalTex, _File);
}

#include "pch.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"


CUIRender::CUIRender()
	: CRenderComponent(COMPONENT_TYPE::UIRENDER)
{
}

CUIRender::~CUIRender()
{
}

void CUIRender::Init()
{
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);
}

void CUIRender::FinalTick()
{
}

void CUIRender::Render()
{
	assert(UI());

	if (CLevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::STOP)
		return;

	// 위치정보
	Transform()->Binding();

	UI()->Binding();

	// 메쉬의 모든 Subset(부위, 인덱스버퍼) 를 렌더링한다.
	for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		// 사용할 쉐이더
		GetMaterial(i)->Binding();

		// 렌더링
		GetMesh()->Render(i);
	}

	// Font Render
	UI()->FontRender();
}

void CUIRender::SaveComponent(FILE* _File)
{
	Ptr<CMesh> pMesh = GetMesh();
	// 메쉬 참조정보 저장
	SaveAssetRef(pMesh, _File);

	// 재질 참조정보 저장
	UINT iMtrlCount = GetMaterialCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveAssetRef(GetSharedMaterial(i), _File);
	}
}

void CUIRender::LoadComponent(FILE* _FILE)
{
	Ptr<CMesh> pMesh = nullptr;

	// 메쉬 참조정보 불러오기
	LoadAssetRef(pMesh, _FILE);
	SetMesh(pMesh);

	// 재질 참조정보 불러오기
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _FILE);

	SetMaterialSize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		Ptr<CMaterial> pShaderMtrl = GetSharedMaterial(i);
		LoadAssetRef(pShaderMtrl, _FILE);
		SetMaterial(pShaderMtrl, i);
	}
}

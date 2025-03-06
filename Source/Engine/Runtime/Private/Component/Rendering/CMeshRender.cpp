#include "pch.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Runtime/Public/Component/Animation/CFlipBookPlayer.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Rendering/Buffer/CStructuredBuffer.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
	  , m_SkinRender(false)
{
}

CMeshRender::~CMeshRender()
{
}

CAnimator3D* CMeshRender::GetAnimator()
{
	if (!m_SkinRender)
		return nullptr;

	CAnimator3D* pAnimator = nullptr;

	if (GetOwner()->GetParent())
	{
		pAnimator = GetOwner()->GetParent()->Animator3D();
	}

	return pAnimator;
}

void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
	if (FlipbookPlayer())
		FlipbookPlayer()->Binding();

	// Animator3D Binding
	CAnimator3D* pAnimator = Animator3D();

	if (pAnimator)
	{
		pAnimator->Binding(this);

		for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetMaterial(i)->SetBoneCount(pAnimator->GetBoneCount());
		}
	}

	// 위치정보
	Transform()->Binding();

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

	if (FlipbookPlayer())
		FlipbookPlayer()->Clear();

	if (pAnimator)
		pAnimator->ClearData();
}


void CMeshRender::SaveComponent(FILE* _File)
{
	fwrite(&m_SkinRender, sizeof(bool), 1, _File);

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

void CMeshRender::LoadComponent(FILE* _FILE)
{
	fread(&m_SkinRender, sizeof(bool), 1, _FILE);

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

void CMeshRender::Render_Skeleton(CStructuredBuffer* _PureBoneBuffer, CStructuredBuffer* _ParentIndexBuffer)
{
	_PureBoneBuffer->Binding(18);
	_ParentIndexBuffer->Binding(19);

	UINT BoneCount = _PureBoneBuffer->GetElementCount();

	// 위치정보
	Transform()->Binding();

	// 사용할 쉐이더
	GetMaterial(0)->SetBoneCount(BoneCount);
	GetMaterial(0)->Binding();

	// 렌더링
	GetMesh()->Render_Cluster_Instancing((BoneCount + 63) / 64);	// bone 64개씩 끊어서 instancing

	// Clear
	_PureBoneBuffer->Clear(18);
	_ParentIndexBuffer->Clear(19);
}

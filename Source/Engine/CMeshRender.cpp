#include "pch.h"
#include "CMeshRender.h"

#include "assets.h"
#include "CTransform.h"
#include "CFlipbookPlayer.h"
#include "CAnimator3D.h"

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

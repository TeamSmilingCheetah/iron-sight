#include "pch.h"
#include "CMeshRender.h"

#include "assets.h"
#include "CTransform.h"
#include "CFlipbookPlayer.h"
#include "CAnimator3D.h"

CMeshRender::CMeshRender()
    : CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
    if (FlipbookPlayer())
        FlipbookPlayer()->Binding();

    // Animator3D Binding
	// 본인에게 붙어 있는 경우
	CAnimator3D* pAnimator = Animator3D();

	// 부모에게 붙어 있는 경우
	if (!pAnimator && m_SkinRender)
	{
		pAnimator = GetOwner()->GetParent()->Animator3D();
	}

	if (m_SkinRender)
	{
		if (pAnimator)
		{
			pAnimator->Binding();

			for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
			{
				if (nullptr == GetMaterial(i))
					continue;

				GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
				GetMaterial(i)->SetBoneCount(pAnimator->GetBoneCount());
			}
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
		pAnimator->ClearData(this);
}

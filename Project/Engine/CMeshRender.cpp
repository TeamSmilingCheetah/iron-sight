#include "pch.h"
#include "CMeshRender.h"

#include "assets.h"
#include "CTransform.h"
#include "CFlipbookPlayer.h"

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
    {
        FlipbookPlayer()->Binding();
    }

    // Animator3D Binding
    if (Animator3D())
    {
        Animator3D()->Binding();

        for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
        {
            if (nullptr == GetMaterial(i))
                continue;

            GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
            GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
        }
    }

    // 위치정보
    Transform()->Binding();

    // 메쉬의 모든 Subset(부위, 인덱스버퍼) 를 렌더링한다.
    for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
    {
        if (nullptr == GetMaterial(i))
            continue;

        // 사용할 쉐이더
        GetMaterial(i)->Binding();

        // 렌더링
        GetMesh()->Render(i);
    }

    if (FlipbookPlayer())
    {
        FlipbookPlayer()->Clear();
    }
}

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

            GetMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
            GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
        }
    }

    // ��ġ����
    Transform()->Binding();

    // �޽��� ��� Subset(����, �ε�������) �� �������Ѵ�.
    for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
    {
        if (nullptr == GetMaterial(i))
            continue;

        // ����� ���̴�
        GetMaterial(i)->Binding();

        // ������
        GetMesh()->Render(i);
    }

    if (FlipbookPlayer())
    {
        FlipbookPlayer()->Clear();
    }
}

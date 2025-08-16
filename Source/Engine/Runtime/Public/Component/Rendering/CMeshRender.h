#pragma once
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"


class CMeshRender :
	public FRenderComponent
{
	friend class CRenderMgr;

private:
	bool	m_SkinRender;	// 애니메이션을 적용해서 bone에 의한 변형을 사용할 건지 여부

public:
	void SetSkinRender(bool _b) { m_SkinRender = _b; }

	bool IsSkinRender() const { return m_SkinRender; }
	CAnimator3D* GetAnimator();

public:
	void FinalTick() override;
	void Render() override;


	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _FILE) override;


private:
	void Render_Skeleton(CStructuredBuffer* _PureBoneBuffer, CStructuredBuffer* _ParentIndexBuffer);	// 인스턴싱 활용한 Skeleton render -> RenderMgr에서만 호출

public:
	CLONE(CMeshRender);
	CMeshRender();
	~CMeshRender() override;
};

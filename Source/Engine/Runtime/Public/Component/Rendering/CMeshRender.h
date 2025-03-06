#pragma once
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"

class CMeshRender :
    public CRenderComponent
{
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

    CLONE(CMeshRender);
    CMeshRender();
    ~CMeshRender() override;
};

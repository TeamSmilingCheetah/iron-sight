#pragma once
#include "CRenderComponent.h"

#include "assets.h"

class CMeshRender :
    public CRenderComponent
{
private:
	bool	m_SkinRender;	// 애니메이션을 적용해서 bone에 의한 변형을 사용할 건지 여부

public:
	void SetSkinRender(bool _b) { m_SkinRender = _b; }

public:
    void FinalTick() override;
    void Render() override;

    CLONE(CMeshRender);
    CMeshRender();
    ~CMeshRender() override;
};

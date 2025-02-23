#pragma once
#include <Engine/CScript.h>

class CCameraScript :
    public CScript
{
private:
    float   m_CameraSpeed;

public:
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

private:
    void CameraOrthgraphicMove();
    void CameraPerspectiveMove();

public:
    CLONE(CCameraScript);
    CCameraScript();
    ~CCameraScript();
};


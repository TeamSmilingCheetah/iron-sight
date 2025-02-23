#pragma once
#include "CComponent.h"

class CFrustum;

class CCamera :
    public CComponent
{
    CFrustum* m_Frustum;

    // ����
    PROJ_TYPE m_ProjType; // ���� ���
    float m_Far; // �þ� �ִ� �Ÿ�
    float m_AspectRatio; // ���� ������ ���� ��Ⱦ��, ���� / ����
    UINT m_LayerCheck; // ī�޶� ���� ���̾� ��Ʈ üũ
    int m_Priority; // ī�޶� �켱����, 0 : MainCamera, -1 : �̵��

    // ��������(Perspective)
    float m_FOV; // FOV(Field Of View) �þ߰�

    // ��������(Orthographic)
    float m_Width; // �������� ���α���
    float m_Scale; // �������� ����

    // ī�޶� �ٶ󺸴� ȭ�鿡�� ���콺�� ���ϴ� ����
    tRay m_Ray;

    // ��ȯ���
    Matrix m_matView; // View ���
    Matrix m_matViewInv; // View �����

    Matrix m_matProj; // Projection ���
    Matrix m_matProjInv; // Projection �����


    // ��ü �з�
    vector<CGameObject*> m_vecDeferred;
    vector<CGameObject*> m_vecDecal;
    vector<CGameObject*> m_vecOpaque;
    vector<CGameObject*> m_vecMask;
    vector<CGameObject*> m_vecTransparent;
    vector<CGameObject*> m_vecEffect;
    vector<CGameObject*> m_vecParticle;
    vector<CGameObject*> m_vecPostprocess;

public:
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }
    void SetWidth(float _Width) { m_Width = _Width; }
    void SetAspectRatio(float _AR) { m_AspectRatio = _AR; }
    void SetScale(float _Scale) { m_Scale = _Scale; }
    void SetFar(float _Far) { m_Far = _Far; }
    void SetFOV(float _FOV) { m_FOV = _FOV; }
    void SetPriority(int _Priority) { m_Priority = _Priority; }

    float GetFar() { return m_Far; }
    float GetFOV() { return m_FOV; }
    float GetWidth() { return m_Width; }
    float GetAspectRatio() { return m_AspectRatio; }
    int GetCameraPriority() { return m_Priority; }
    float GetScale() { return m_Scale; }
    PROJ_TYPE GetProjType() { return m_ProjType; }

    const tRay& GetRay() { return m_Ray; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetViewInvMat() { return m_matViewInv; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetProjInvMat() { return m_matProjInv; }

    void LayerCheck(int _LayerIdx);
    void LayerCheckAll() { m_LayerCheck = 0xffffffff; }
    void LayerCheckClear() { m_LayerCheck = 0; }


    void Begin() override;
    void FinalTick() override;

    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _FILE) override;

    void SortObject();
    void render_deferred();
    void render_decal();
    void render_forward();
    void render_effect();
    void render_particle();
    void render_postprocess();
    void render_clear();

private:
    bool FrustumCheck(CGameObject* _Object);
    void CalcRay();

public:
    CLONE(CCamera);
    CCamera();
    CCamera(const CCamera& _Origin);
    ~CCamera() override;
};

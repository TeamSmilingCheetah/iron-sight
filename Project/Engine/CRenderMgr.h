#pragma once
#include "singleton.h"


#include "CTexture.h"
#include "CMaterial.h"

class CCamera;
class CLight2D;
class CLight3D;
class CStructuredBuffer;
class CMRT;

class CRenderMgr :
    public singleton<CRenderMgr>
{
    SINGLE(CRenderMgr);
    CMRT* m_arrMRT[static_cast<UINT>(MRT_TYPE::END)];

    vector<CCamera*> m_vecCam; // ���� ������ ������ ���� �ȿ��ִ� ī�޶��
    CCamera* m_EditorCam; // ������ ������ ���� �ܺ� ī�޶�

    vector<CLight2D*> m_vecLight2D;
    CStructuredBuffer* m_Light2DBuffer;

    vector<CLight3D*> m_vecLight3D;
    CStructuredBuffer* m_Light3DBuffer;

    list<tDebugShapeInfo> m_DbgList; // ����� ������ ����
    class CGameObject* m_DbgObj; // ����� ������ ������Ʈ

    Ptr<CTexture> m_PostProcessTex; // ��ó���� �ؽ���(����Ÿ�� ����뵵)

    bool m_IsEditor; // �����͸�� �� or �ΰ��� ��

    Ptr<CTexture> m_SpecifyTarget; // ��½�ų ����Ÿ��
    Ptr<CMaterial> m_MergeMtrl;

    bool m_DebugRender;

public:
    void RegisterCamera(CCamera* _Cam, UINT _Priority);
    void RegisterEditorCamera(CCamera* _EditorCam) { m_EditorCam = _EditorCam; }
    void DeregisterCamera() { m_vecCam.clear(); }

    void RegisterLight2D(CLight2D* _Light2D) { m_vecLight2D.push_back(_Light2D); }

    int RegisterLight3D(CLight3D* _Light3D)
    {
        m_vecLight3D.push_back(_Light3D);
        return static_cast<int>(m_vecLight3D.size()) - 1;
    }

    void AddDebugShape(const tDebugShapeInfo& _info) { m_DbgList.push_back(_info); }

    void CopyRenderTarget();
    void SetEditorMode(bool _IsEditor) { m_IsEditor = _IsEditor; }

    void SetSpecifyTarget(Ptr<CTexture> _Target) { m_SpecifyTarget = _Target; }

    CCamera* GetMainCamera();
    CMRT* GetMRT(MRT_TYPE _Type) { return m_arrMRT[static_cast<UINT>(_Type)]; }

    void Init();
    void Render();

private:
    void RenderStart();
    void Binding();
    void ClearMRT();
    void Render_Debug();

    void Render_Play();
    void Render_Editor();

    void Render_Clear();

    void MergeDeferredTarget();
    void MergeSpecifyTarget();

    void CreateMRT();
    void CreateDebugMtrl();
    void CreateRenderMtrl();
};

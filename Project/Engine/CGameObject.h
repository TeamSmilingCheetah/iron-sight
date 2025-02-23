#pragma once
#include "CEntity.h"


class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
    CComponent* m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
    CRenderComponent* m_RenderCom; // 1���� ���ӿ�����Ʈ ��ü�� 1���� ����������Ʈ�� ���� �� �ִ�.
    vector<CScript*> m_vecScripts; // ���� ��ũ��Ʈ��

    CGameObject* m_Parent; // �θ� ������Ʈ
    vector<CGameObject*> m_vecChild; // �ڽ� ������Ʈ��

    int m_LayerIdx; // ������Ʈ�� �����ִ� ���̾� �ε��� ��ȣ, -1 : ���Ҽ�
    bool m_Dead; // ������Ʈ�� ���°� ���� ���� ��������

public:
    void Begin();
    void Tick();
    void FinalTick();
    void Render();

    void AddComponent(CComponent* _Component);
    void AddChild(CGameObject* _Child);

    CGameObject* GetParent() { return m_Parent; }
    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[static_cast<UINT>(_Type)]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }

    int GetLayerIdx() { return m_LayerIdx; }
    bool IsDead() { return m_Dead; }
    bool IsAncestor(CGameObject* _Other);

    const vector<CGameObject*>& GetChild() { return m_vecChild; }
    const vector<CScript*>& GetScripts() { return m_vecScripts; }

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }

    class CMeshRender* MeshRender()
    {
        return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER);
    }

    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }

    class CCollider2D* Collider2D()
    {
        return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D);
    }

    class CFlipbookPlayer* FlipbookPlayer()
    {
        return (CFlipbookPlayer*)GetComponent(COMPONENT_TYPE::FLIPBOOKPLAYER);
    }

    class CTileMap* TileMap() { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
    class CLight2D* Light2D() { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
    class CLight3D* Light3D() { return (CLight3D*)GetComponent(COMPONENT_TYPE::LIGHT3D); }

    class CAnimator3D* Animator3D()
    {
        return (CAnimator3D*)GetComponent(COMPONENT_TYPE::ANIMATOR3D);
    }

    class CParticleSystem* ParticleSystem()
    {
        return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
    }

    class CSkyBox* SkyBox() { return (CSkyBox*)GetComponent(COMPONENT_TYPE::SKYBOX); }
    class CDecal* Decal() { return (CDecal*)GetComponent(COMPONENT_TYPE::DECAL); }
    class CLandScape* LandScape() { return (CLandScape*)GetComponent(COMPONENT_TYPE::LANDSCAPE); }

private:
    void DisconnectWithLayer();
    void DisconnecntWithParent();
    void RegisterAsParent();

public:
    CLONE(CGameObject);
    CGameObject();
    CGameObject(const CGameObject& _Origin);
    ~CGameObject() override;

    friend class CLayer;
    friend class CTaskMgr;
};

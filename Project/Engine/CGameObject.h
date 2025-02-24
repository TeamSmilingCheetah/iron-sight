#pragma once
#include "CEntity.h"


class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
    CComponent* m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
    CRenderComponent* m_RenderCom; // 1개의 게임오브젝트 객체는 1개의 렌더컴포넌트를 가질 수 있다.
    vector<CScript*> m_vecScripts; // 보유 스크립트들

    CGameObject* m_Parent; // 부모 오브젝트
    vector<CGameObject*> m_vecChild; // 자식 오브젝트들

    int m_LayerIdx; // 오브젝트가 속해있는 레이어 인덱스 번호, -1 : 무소속
    bool m_Dead; // 오브젝트의 상태가 삭제 예정 상태인지

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

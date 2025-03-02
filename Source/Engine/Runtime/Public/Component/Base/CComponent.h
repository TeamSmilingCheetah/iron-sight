#pragma once
#include "Engine/Core/Public/CEntity.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"

#define GET_OTHER_COMPONENT(Type) C##Type* Type() { return m_Owner->Type(); }

class CComponent :
    public CEntity
{
    CGameObject* m_Owner;
    const COMPONENT_TYPE m_Type;

public:
    COMPONENT_TYPE GetType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
	GET_OTHER_COMPONENT(Collider3D);
    GET_OTHER_COMPONENT(FlipbookPlayer);
    GET_OTHER_COMPONENT(TileMap);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(Light3D);
    GET_OTHER_COMPONENT(Animator3D);
    GET_OTHER_COMPONENT(ParticleSystem);
    GET_OTHER_COMPONENT(SkyBox);
    GET_OTHER_COMPONENT(Decal);
    GET_OTHER_COMPONENT(LandScape);

    virtual void Init()
    {
    } // 생성자 시점 이후에, GameObject 에 AddComponent 된 직후 호출되는 함수

	virtual void Begin()
    {
    }

    virtual void Tick()
    {
    }
    virtual void FinalTick() = 0;

    CComponent* Clone() override = 0;

    void SaveToLevel(FILE* _File) override;
    void LoadFromLevel(FILE* _File) override;

    virtual void SaveComponent(FILE* _File) = 0;
    virtual void LoadComponent(FILE* _File) = 0;

    CComponent(COMPONENT_TYPE _TYPE);
    ~CComponent() override;

    friend class CGameObject;
};

#include "pch.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLayer.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Component/Base/CRenderComponent.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Runtime/Public/Component/Script/CScript.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Scripts/Manager/CScriptMgr.h"

CGameObject::CGameObject()
    : m_arrCom{}
      , m_RenderCom(nullptr)
      , m_Parent(nullptr)
      , m_LayerIdx(-1) // -1 == 특정 레이어에 소속이 아니다 --> Level 안에 존재하지 않은 상태
      , m_Dead(false)
{
    // Transform 컴포넌트는 무조건 가져야 되는 기본 컴포넌트
    AddComponent(new CTransform);
}

CGameObject::CGameObject(const CGameObject& _Origin)
    : CEntity(_Origin)
      , m_arrCom{}
      , m_RenderCom(nullptr)
      , m_Parent(nullptr)
      , m_LayerIdx(-1)
      , m_Dead(false)
{
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (nullptr == _Origin.m_arrCom[i])
            continue;

        AddComponent(_Origin.m_arrCom[i]->Clone());
    }

    for (size_t i = 0; i < _Origin.m_vecScripts.size(); ++i)
    {
        AddComponent(_Origin.m_vecScripts[i]->Clone());
    }

    for (size_t i = 0; i < _Origin.m_vecChild.size(); ++i)
    {
        AddChild(_Origin.m_vecChild[i]->Clone());
    }
}

CGameObject::~CGameObject()
{
    DeleteArray(m_arrCom);

    DeleteVec(m_vecScripts);

    DeleteVec(m_vecChild);
}

void CGameObject::Begin()
{
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (!m_arrCom[i])
            continue;

        m_arrCom[i]->Begin();
    }

    for (size_t i = 0; i < m_vecScripts.size(); ++i)
    {
        m_vecScripts[i]->Begin();
    }


    for (size_t i = 0; i < m_vecChild.size(); ++i)
    {
        m_vecChild[i]->Begin();
    }
}

void CGameObject::Tick()
{
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (!m_arrCom[i])
            continue;

        m_arrCom[i]->Tick();
    }

    for (size_t i = 0; i < m_vecScripts.size(); ++i)
    {
        m_vecScripts[i]->Tick();
    }

    for (size_t i = 0; i < m_vecChild.size(); ++i)
    {
        m_vecChild[i]->Tick();
    }
}

void CGameObject::FinalTick()
{
    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (!m_arrCom[i])
            continue;

        m_arrCom[i]->FinalTick();
    }

    auto iter = m_vecChild.begin();
    for (; iter != m_vecChild.end();)
    {
        (*iter)->FinalTick();

        if ((*iter)->IsDead())
        {
            iter = m_vecChild.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    // Layer 등록
    CLevelMgr::GetInst()->RegisterObject(this);
}

void CGameObject::Render()
{
    m_RenderCom->Render();
}

void CGameObject::AddComponent(CComponent* _Component)
{
    COMPONENT_TYPE Type = _Component->GetType();


    if (COMPONENT_TYPE::SCRIPT == Type)
    {
        m_vecScripts.push_back(static_cast<CScript*>(_Component));
    }
    else
    {
        // 입력으로 들어오는 컴포넌트와 이미 동일한 컴포넌트를 오브젝트가 가지고 있는 경우
        assert(!m_arrCom[static_cast<UINT>(Type)]);

        // 입력된 컴포넌트가 CRenderComponent 의 자식클래스 타입인지 확인
        if (dynamic_cast<CRenderComponent*>(_Component))
        {
            assert(!m_RenderCom);
            m_RenderCom = static_cast<CRenderComponent*>(_Component);
        }

        // 입력된 컴포넌트의 주소를 저장
        m_arrCom[static_cast<UINT>(Type)] = _Component;
    }

    // 컴포넌트의 소유오브젝트를 세팅
    _Component->m_Owner = this;

    // 컴포넌트 초기화
    _Component->Init();
}

void CGameObject::AddChild(CGameObject* _Child)
{
    m_vecChild.push_back(_Child);
    _Child->m_Parent = this;
}

void CGameObject::DeleteComponent(COMPONENT_TYPE _Type)
{
	if (COMPONENT_TYPE::SCRIPT == _Type)
	{
		return;
	}
	else
	{
		// 입력으로 들어오는 컴포넌트가 없는 경우
		assert(m_arrCom[(UINT)_Type]);

		// 입력된 컴포넌트가 CRenderComponent 의 자식클래스 타입인지 확인
		if (_Type == COMPONENT_TYPE::TILEMAP || _Type == COMPONENT_TYPE::MESHRENDER || _Type == COMPONENT_TYPE::PARTICLE_SYSTEM)
		{
			assert(m_RenderCom);
			m_RenderCom = nullptr;
		}

		// 해당 컴퍼넌트 삭제
		m_arrCom[(UINT)_Type] = nullptr;
	}
}

void CGameObject::DeleteScirpt(wstring& _SciprtName)
{
	CScript* pScript = CScriptMgr::GetScript(_SciprtName);

	for (UINT i = 0; i < m_vecScripts.size(); ++i)
	{
		if (m_vecScripts[i]->GetName() == pScript->GetName())
			m_vecScripts.erase(m_vecScripts.begin() + i);
	}
}

bool CGameObject::IsAncestor(CGameObject* _Other)
{
    CGameObject* pParent = m_Parent;

    while (pParent)
    {
        if (pParent == _Other)
            return true;

        pParent = pParent->m_Parent;
    }

    return false;
}

CAnimator3D* CGameObject::Animator3D()
{
	CAnimator3D* pAnimator = (CAnimator3D*)GetComponent(COMPONENT_TYPE::ANIMATOR3D);

	if (!pAnimator && MeshRender() && MeshRender()->IsSkinRender())
		pAnimator = MeshRender()->GetAnimator();

	return pAnimator;
}

void CGameObject::DisconnectWithLayer()
{
    // 소속 레이어가 없다면
    if (-1 == m_LayerIdx)
        return;

    CLayer* pLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
    pLayer->DisconnectObject(this);
}

void CGameObject::DisconnecntWithParent()
{
    if (nullptr == m_Parent)
        return;

    auto iter = m_Parent->m_vecChild.begin();

    for (; iter != m_Parent->m_vecChild.end(); ++iter)
    {
        if (*iter == this)
        {
            m_Parent->m_vecChild.erase(iter);
            m_Parent = nullptr;
            return;
        }
    }

    assert(nullptr);
}

void CGameObject::RegisterAsParent()
{
    // 소속 레이어가 없다면
    if (-1 == m_LayerIdx)
        return;

    CLayer* pLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(m_LayerIdx);
    pLayer->RegisterAsParent(this);
}

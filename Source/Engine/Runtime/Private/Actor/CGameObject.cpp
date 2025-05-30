#include "pch.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLayer.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Component/Base/CRenderComponent.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Runtime/Public/Component/Script/CScript.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Game/System/Public/GameplayManager.h"

UINT CGameObject::GUID = 0;

CGameObject::CGameObject()
	: m_ObjectID(GUID++)
	, m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_LayerIdx(-1) // -1 == 특정 레이어에 소속이 아니다 --> Level 안에 존재하지 않은 상태
	, m_NextLayerIdx(-1)
	, m_Active(true)
	, m_Dead(false)
	, m_Deactivate(false)
	, m_LayerMove(false)
{
	// Transform 컴포넌트는 무조건 가져야 되는 기본 컴포넌트
	AddComponent(new CTransform);
}

CGameObject::CGameObject(const CGameObject& _Origin)
	: CEntity(_Origin)
	, m_ObjectID(GUID++)
	, m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_Active(_Origin.m_Active)
	, m_Dead(false)
{
	for (size_t i = 0; i < _Origin.m_vecChild.size(); ++i)
	{
		AddChild(_Origin.m_vecChild[i]->Clone());
	}

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
	if (!m_Active)
		return;

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
	if (!m_Active)
		return;

	// Layer 등록
	CLevelMgr::GetInst()->RegisterObject(this);

	if (m_Parent != nullptr && m_Parent->IsDead())
	{
		m_Dead = true;
	}

	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (!m_arrCom[i])
			continue;

		m_arrCom[i]->FinalTick();
	}

	auto iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();)
	{
		bool ChildDead = (*iter)->IsDead();

		(*iter)->FinalTick();

		if (ChildDead)
		{
			iter = m_vecChild.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CGameObject::Render()
{
	if (!m_Active)
		return;

	m_RenderCom->Render();
}

void CGameObject::AddComponent(CComponent* _Component)
{
	COMPONENT_TYPE Type = _Component->GetType();

	if (COMPONENT_TYPE::SCRIPT == Type)
	{
		// 오브젝트에 script 추가
		m_vecScripts.push_back(static_cast<CScript*>(_Component));
		// script가 몇 번 위치에 존재하는지 기록
		m_scriptShortcut[static_cast<CScript*>(_Component)->GetScriptType()] = static_cast<UINT>(m_vecScripts.size()) - 1;

		// 부모 스크립트 정보로 들고 있는 경우, 해당 정보도 Shortcut에 등록해야 조회 시 문제가 없다
		if (static_cast<CScript*>(_Component)->GetParentScriptType() != SCRIPT_TYPE::NONE)
		{
			m_scriptShortcut[static_cast<CScript*>(_Component)->GetParentScriptType()] = static_cast<UINT>(m_vecScripts.size()) - 1;
		}
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
	_Component->SetOwner(this);

	// 컴포넌트 초기화
	_Component->Init();
}

void CGameObject::AddChild(CGameObject* _Child)
{
	m_vecChild.push_back(_Child);
	_Child->m_Parent = this;
	_Child->m_LayerIdx = m_LayerIdx;
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
		if (nullptr != m_arrCom[(UINT)_Type])
			delete m_arrCom[(UINT)_Type];

		m_arrCom[(UINT)_Type] = nullptr;
	}
}

void CGameObject::DeleteScript(wstring& _ScriptName)
{
	CScript* pScript = GameplayManager::GetScript(_ScriptName);

	for (UINT i = 0; i < m_vecScripts.size(); ++i)
	{
		if (m_vecScripts[i]->GetName() == pScript->GetName())
		{
			// 해당 스크립트 관련 정보 일괄 제거
			m_vecScripts.erase(m_vecScripts.begin() + i);
			m_scriptShortcut.erase(pScript->GetScriptType());

			// 부모 스크립트 정보로 들고 있는 경우, Trailing Delete
			if (pScript->GetParentScriptType() != SCRIPT_TYPE::NONE)
			{
				m_scriptShortcut.erase(pScript->GetParentScriptType());
			}
		}
	}
}
void CGameObject::SetObjectID(UINT _ID)
{
	m_ObjectID = _ID;
	GUID = max(GUID, _ID + 1);
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

//CScript* CGameObject::GetScript(SCRIPT_TYPE _Type) const
//{
//	for (CScript* script : m_vecScripts)
//	{
//		if (script->GetScriptType() == _Type)
//			return script;
//	}
//
//	return nullptr;
//}

CScript* CGameObject::GetParentScript(SCRIPT_TYPE _Type) const
{
	for (CScript* script : m_vecScripts)
	{
		if (script->GetParentScriptType() == _Type)
			return script;
	}

	return nullptr;
}

CGameObject* CGameObject::GetChildByName(const wstring& _Name)
{
	queue<CGameObject*> Q;
	Q.emplace(this);

	while (!Q.empty())
	{
		CGameObject* pObj = Q.front();
		Q.pop();

		if (pObj->GetName() == _Name)
		{
			return pObj;
		}

		for (CGameObject* pChild : pObj->m_vecChild)
		{
			Q.emplace(pChild);
		}
	}

	return nullptr;
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

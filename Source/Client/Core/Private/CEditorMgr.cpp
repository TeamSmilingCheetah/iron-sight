#include "pch.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Client/Core/Public/CEditorMgr.h"
#include "Client/Script/Public/CEditorCamScript.h"
#include "Client/Script/Public/CGameObjectEx.h"

CEditorMgr::CEditorMgr()
{
}

CEditorMgr::~CEditorMgr()
{
	DeleteVec(m_vecEditorObj);
}

void CEditorMgr::Init()
{
	auto pObject = new CGameObjectEx;
	pObject->SetName(L"EditorCamera");
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CEditorCamScript);

	pObject->Transform()->SetRelativePos(0.f, 0.f, -200.f);
	pObject->Camera()->LayerCheckAll();
	pObject->Camera()->SetProjType(PERSPECTIVE);
	pObject->Camera()->SetFar(100000.f);

	m_vecEditorObj.push_back(pObject);

	// Editor 용 카메라 등록
	CRenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());
}

void CEditorMgr::Progress()
{
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->FinalTick_Editor();
	}
}

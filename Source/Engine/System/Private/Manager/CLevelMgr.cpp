#include "pch.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CTimeMgr.h"


#include "Engine/Runtime/Public/Component/Base/components.h"
#include "Engine/System/Public/Manager/CScriptMgr.h"
#include "System/Public/Manager/CollisionManager.h"

CLevelMgr::CLevelMgr()
	: m_CurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	DELETE(m_CurLevel);
	LOG_INFO("[Engine][LevelManager] Instance Deleted");
}

void CLevelMgr::Init()
{
}

void CLevelMgr::Progress()
{
	if (nullptr == m_CurLevel)
		return;

	m_CurLevel->RegisterClear();

	if (m_CurLevel->GetState() == LEVEL_STATE::PLAY)
	{
		m_CurLevel->Tick();
	}

	m_CurLevel->FinalTick();
}

void CLevelMgr::RegisterObject(CGameObject* _Object)
{
	int LayerIdx = _Object->GetLayerIdx();
	CLayer* pLayer = m_CurLevel->GetLayer(LayerIdx);
	pLayer->RegisterObject(_Object);
}

CGameObject* CLevelMgr::FindObjectByName(const wstring& _Name)
{
	if (nullptr == m_CurLevel)
		return nullptr;

	return m_CurLevel->FindObjectByName(_Name);
}

CGameObject* CLevelMgr::FindObjectByID(UINT _ID)
{
	if (nullptr == m_CurLevel)
		return nullptr;

	return m_CurLevel->FindObjectByObjectID(_ID);
}

void CLevelMgr::ChangeLevelState(LEVEL_STATE _NextState)
{
	assert(m_CurLevel);
	if (m_CurLevel->GetState() == _NextState)
		return;

	// Play or None ->  Pause
	//			    ㄴ> Stop
	if (_NextState == LEVEL_STATE::PAUSE || _NextState == LEVEL_STATE::STOP)
	{
		CTimeMgr::GetInst()->SetStopMode(true);
		CRenderMgr::GetInst()->SetEditorMode(true);

		if (LEVEL_STATE::STOP == _NextState)
			CRenderMgr::GetInst()->DeregisterCamera();
	}

	// Pause, Stop -> Play
	else if (_NextState == LEVEL_STATE::PLAY)
	{
		CTimeMgr::GetInst()->SetStopMode(false);
		CRenderMgr::GetInst()->SetEditorMode(false);

		// 레벨이 Play될 때 오브젝트 풀 Preload 실행
		ExcutePreload();
	}

	// Stop -> Play
	if ((m_CurLevel->GetState() == LEVEL_STATE::STOP || m_CurLevel->GetState() == LEVEL_STATE::NONE)
		&& _NextState == LEVEL_STATE::PLAY)
	{
		m_CurLevel->ChangeState(_NextState);
		m_CurLevel->Begin();
	}
	else
	{
		m_CurLevel->ChangeState(_NextState);
	}
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel, LEVEL_STATE _NextLevelState)
{
	CLevel* pPrevLevel = m_CurLevel;

	// 이전 레벨은 삭제
	if (pPrevLevel)
	{
		FCollisionManager::GetInst()->ClearPreviousLevelInformation();
		delete pPrevLevel;
	}

	m_CurLevel = _NextLevel;
	ChangeLevelState(_NextLevelState);

	// Init Callback 호출
	for (const auto& func : m_LevelInitCallback)
	{
		func();
	}
}

void CLevelMgr::ResolveReference(CLevel* _Level)
{
	for (int i = 0; i < m_ReferenceResolution.size(); ++i)
	{
		m_ReferenceResolution[i].MissingAddress = _Level->FindObjectByObjectID(m_ReferenceResolution[i].ObjectID);
	}

	m_ReferenceResolution.clear();
}

void CLevelMgr::AddObjectRefResolution(CGameObject*& _MissingAddress, UINT _ObjectID)
{
	m_ReferenceResolution.push_back(tObjectRefResolution{ _MissingAddress, _ObjectID });
}

int CLevelMgr::SaveLevel(const wstring& _FilePath, CLevel* _Level)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");
	assert(pFile);

	// Level 이름 저장
	_Level->SaveToLevel(pFile);

	// Level 이 보유하고있는 32개의 레이어를 저장시킨다.
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = _Level->GetLayer(i);

		// 레이어 이름 저장
		pLayer->SaveToLevel(pFile);

		// 레이어가 소유한 오브젝트 정보 저장
		const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

		// 오브젝트 총 개수
		size_t ObjectCount = vecObjects.size();
		fwrite(&ObjectCount, sizeof(size_t), 1, pFile);

		// 각 오브젝트의 정보
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			SaveGameObject(vecObjects[j], pFile);
		}
	}

	fclose(pFile);

	return S_OK;
}

int CLevelMgr::SaveGameObject(CGameObject* _Object, FILE* _File)
{
	// 오브젝트 이름
	_Object->SaveToLevel(_File);

	// 오브젝트 id 저장
	UINT ID = _Object->GetObjectID();
	fwrite(&ID, sizeof(UINT), 1, _File);

	// 오브젝트 활성화 여부 저장
	bool active = _Object->IsActive();
	fwrite(&active, sizeof(bool), 1, _File);

	// 오브젝트 컴포넌트
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		FComponent* pComponent = _Object->GetComponent(static_cast<COMPONENT_TYPE>(i));
		if (nullptr == pComponent)
			continue;

		// 컴포넌트 타입 저장
		fwrite(&i, sizeof(UINT), 1, _File);

		// 컴포넌트 데이터 저장
		_Object->GetComponent(static_cast<COMPONENT_TYPE>(i))->SaveToLevel(_File);
	}

	UINT End = static_cast<UINT>(COMPONENT_TYPE::SAVE_END);
	fwrite(&End, sizeof(UINT), 1, _File);


	// 오브젝트 스크립트
	const vector<CScript*>& vecScripts = _Object->GetScripts();
	size_t ScriptCount = vecScripts.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		// Script 클래스 이름 저장
		wstring ScriptName = CScriptMgr::GetInst()->GetScriptName(vecScripts[i]->GetScriptType());
		SaveWString(ScriptName, _File);

		// Script 가 저장해야할 데이터 저장
		vecScripts[i]->SaveToLevel(_File);
	}

	// 자식 오브젝트
	const vector<CGameObject*>& vecChild = _Object->GetChild();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveGameObject(vecChild[i], _File);
	}

	return S_OK;
}

CLevel* CLevelMgr::LoadLevel(const wstring& _FilePath)
{
	FILE* pFile = nullptr;

	CLevel* pNewLevel = new CLevel;

	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");
	assert(pFile);

	if (!pFile)
	{
		LOG_TRACE("[Level][Load] Failed To Load Lv File");
		return nullptr;
	}

	// Level 이름 불러오기
	pNewLevel->LoadFromLevel(pFile);

	// Level 이 보유하고있는 32개의 레이어를 불러온다.
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pNewLevel->GetLayer(i);

		// 레이어 이름 불러오기
		pLayer->LoadFromLevel(pFile);

		// 오브젝트 총 개수
		size_t ObjectCount = 0;
		fread(&ObjectCount, sizeof(size_t), 1, pFile);

		// 각 오브젝트의 정보
		for (size_t j = 0; j < ObjectCount; ++j)
		{
			CGameObject* pNewObject = LoadGameObject(pFile);
			pLayer->AddObject(pNewObject, false);
		}
	}

	fclose(pFile);

	// 모든 Object를 로드했으므로 Object Reference 관계를 처리함
	CLevelMgr::GetInst()->ResolveReference(pNewLevel);

	// 레퍼런스 관계를 처리한 이후에 처리할 로직을 처리하는 시점
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pNewLevel->GetLayer(i);

		// 레이어가 소유한 오브젝트 불러오기
		const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

		queue<CGameObject*> Q;

		// 각 오브젝트의 컴포넌트들의 reference 정보를 처리하는 함수 호출
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			Q.emplace(vecObjects[j]);
		}

		while (!Q.empty())
		{
			CGameObject* curObject = Q.front();
			Q.pop();

			// 자식들 Q에 등록
			const vector<CGameObject*> vecChild = curObject->GetChild();
			for (CGameObject* child : vecChild)
			{
				Q.emplace(child);
			}

			// 컴포넌트에는 오브젝트 레퍼런스 없다고 가정. 스크립트에만 있다고 가정
			//for (UINT k = 0; k < static_cast<UINT>(COMPONENT_TYPE::END); ++k)
			//{
			//	CComponent* pComponent = vecObjects[j]->GetComponent(static_cast<COMPONENT_TYPE>(k));
			//	if (pComponent)
			//		pComponent->LoadComponentReference();
			//}


			const vector<CScript*> vecScripts = curObject->GetScripts();

			for (size_t k = 0; k < vecScripts.size(); ++k)
			{
				vecScripts[k]->LoadComponentReference();
			}
		}

	}

	return pNewLevel;
}

CGameObject* CLevelMgr::LoadGameObject(FILE* _File)
{
	CGameObject* pObject = new CGameObject;

	// 오브젝트 이름
	pObject->LoadFromLevel(_File);

	// 오브젝트 ID
	UINT ID = 0;
	fread(&ID, sizeof(UINT), 1, _File);
	pObject->SetObjectID(ID);

	// 오브젝트 활성화 여부 로드
	bool active = true;
	fread(&active, sizeof(bool), 1, _File);
	pObject->SetActive(active);

	// 오브젝트 컴포넌트
	UINT ComponentType = 0;
	while (true)
	{
		// 컴포넌트 타입 로딩
		fread(&ComponentType, sizeof(UINT), 1, _File);

		if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::SAVE_END))
			break;

		FComponent* pComponent = CreateComponent(static_cast<COMPONENT_TYPE>(ComponentType));

		if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::TRANSFORM))
			pComponent = pObject->Transform();
		else
			pObject->AddComponent(pComponent);

		// 컴포넌트 데이터 저장
		pComponent->LoadFromLevel(_File);
	}

	// 오브젝트 스크립트
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		// Script 클래스 이름을 읽는다.
		wstring ScriptName;
		LoadWString(ScriptName, _File);

		// Script 이름으로 해당 스크립트 객체를 생성 후 GameObject 에 넣어준다.
		CScript* pScript = CScriptMgr::GetInst()->GetScript(ScriptName);
		pObject->AddComponent(pScript);

		// Script 가 저장한 데이터를 다시 복구시킨다.
		pScript->LoadFromLevel(_File);
	}

	// 자식 오브젝트
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		CGameObject* pChild = LoadGameObject(_File);
		pObject->AddChild(pChild);
	}

	return pObject;
}

FComponent* CLevelMgr::CreateComponent(COMPONENT_TYPE _Type)
{
	switch (_Type)
	{
	case COMPONENT_TYPE::PLANE_COLLIDER:
		return new FPlaneCollider;
	case COMPONENT_TYPE::BOX_COLLIDER:
		return new FBoxCollider;
	case COMPONENT_TYPE::FLIPBOOKPLAYER:
		return new CFlipbookPlayer;
	case COMPONENT_TYPE::ANIMATOR3D:
		return new CAnimator3D;
	case COMPONENT_TYPE::CAMERA:
		return new CCamera;
	case COMPONENT_TYPE::LIGHT2D:
		return new CLight2D;
	case COMPONENT_TYPE::LIGHT3D:
		return new CLight3D;
	case COMPONENT_TYPE::STATEMACINE:
		return new CStateMachine;
	case COMPONENT_TYPE::MESHRENDER:
		return new CMeshRender;
	case COMPONENT_TYPE::TILEMAP:
		return new CTileMap;
	case COMPONENT_TYPE::PARTICLE_SYSTEM:
		return new CParticleSystem;
	case COMPONENT_TYPE::SKYBOX:
		return new CSkyBox;
	case COMPONENT_TYPE::DECAL:
		return new CDecal;
	case COMPONENT_TYPE::LANDSCAPE:
		return new FLandscape;
	case COMPONENT_TYPE::RAY_COLLIDER:
		return new FRayCollider;
	case COMPONENT_TYPE::UI:
		return new CUI;
	case COMPONENT_TYPE::UIRENDER:
		return new CUIRender;
	case COMPONENT_TYPE::MESH_COLLIDER:
		return new FMeshCollider;
	case COMPONENT_TYPE::SPHERE_COLLIDER:
		return new FSphereCollider;
	}

	return nullptr;
}

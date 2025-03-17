#include "pch.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CTimeMgr.h"

#include "Engine/Runtime/Public/Component/Base/components.h"
#include "Game/System/Public/GameplayManager.h"

CLevelMgr::CLevelMgr()
	: m_CurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	DELETE(m_CurLevel);
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

	m_CurLevel = _NextLevel;

	ChangeLevelState(_NextLevelState);

	// 이전 레벨은 삭제
	if (nullptr != pPrevLevel)
		delete pPrevLevel;
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
		CComponent* pComponent = _Object->GetComponent(static_cast<COMPONENT_TYPE>(i));
		if (nullptr == pComponent)
			continue;

		// 컴포넌트 타입 저장
		fwrite(&i, sizeof(UINT), 1, _File);

		// 컴포넌트 데이터 저장
		_Object->GetComponent(static_cast<COMPONENT_TYPE>(i))->SaveToLevel(_File);
	}
	UINT End = static_cast<UINT>(COMPONENT_TYPE::END);
	fwrite(&End, sizeof(UINT), 1, _File);


	// 오브젝트 스크립트
	const vector<CScript*>& vecScripts = _Object->GetScripts();
	size_t ScriptCount = vecScripts.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		// Script 클래스 이름 저장
		wstring ScriptName = GameplayManager::GetScriptName(vecScripts[i]);
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

	// Level 이름 불러오기
	pNewLevel->LoadFromLevel(pFile);

	// Level 이 보유하고있는 32개의 레이어를 불러온다.
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pNewLevel->GetLayer(i);

		// 레이어 이름 불러오기
		pLayer->LoadFromLevel(pFile);

		// 레이어가 소유한 오브젝트 불러오기
		const vector<CGameObject*>& vecObjects = pLayer->GetParentObjects();

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

	// 모든 OBject를 로드했으므로 Object Reference 관계를 처리함
	CLevelMgr::GetInst()->ResolveReference(pNewLevel);

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

		if (ComponentType == static_cast<UINT>(COMPONENT_TYPE::END))
			break;

		CComponent* pComponent = CreateComponent(static_cast<COMPONENT_TYPE>(ComponentType));

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
		CScript* pScript = GameplayManager::GetScript(ScriptName);
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

CComponent* CLevelMgr::CreateComponent(COMPONENT_TYPE _Type)
{
	switch (_Type)
	{
	case COMPONENT_TYPE::COLLIDER2D:
		return new CCollider2D;
	case COMPONENT_TYPE::COLLIDER3D:
		return new CCollider3D;
	case COMPONENT_TYPE::COLLIDERRAY:
		return new CColliderRay;
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
		return new CLandScape;
	}

	return nullptr;
}

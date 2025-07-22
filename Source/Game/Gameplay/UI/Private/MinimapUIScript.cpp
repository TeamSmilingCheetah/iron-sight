#include "pch.h"
#include "Game/Gameplay/UI/Public/MinimapUIScript.h"

#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"

#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/Runtime/Public/Component/Rendering/CUIRender.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"

#include "Game/Gameplay/Character/Public/PlayerCharacter.h"
#include "Game/Gameplay/Character/Public/EnemyController.h"

MinimapUIScript::MinimapUIScript()
	: CScript(SCRIPT_TYPE::MINIMAPUI)
	, m_Player(nullptr)
	, m_MinimapRange(3000)
{
}

MinimapUIScript::~MinimapUIScript()
{
}

void MinimapUIScript::Begin()
{
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");

	m_PlayerDot = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"PlayerDot");

	// 플레이어 점 UI 생성
	if (!m_PlayerDot)
	{
		m_PlayerDot = new CGameObject;
		m_PlayerDot->SetName(L"PlayerDot");
		m_PlayerDot->AddComponent(new CUI);
		m_PlayerDot->AddComponent(new CUIRender);

		// 플레이어는 초록색 점
		m_PlayerDot->UI()->SetColor(Vec4(0.f, 1.f, 0.f, 1.f));
		m_PlayerDot->UI()->SetRectSize(Vec2(6.f, 6.f));
		m_PlayerDot->UI()->SetRectPos(Vec2(0.f, 0.f));

		// 미니맵 UI에 자식으로 추가
		GetOwner()->AddChild(m_PlayerDot);
	}

	// 적 점 등록(적 레이어는 7번)
	//const vector<CGameObject*>& VecObject = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(7)->GetObjects();
	//for (auto& EnemyObj : VecObject)
	//{
	//	EnemyController* EScript = static_cast<EnemyController*>(GetScriptWithType(EnemyObj, SCRIPT_TYPE::ENEMYCONTROLLER));
	//
	//	if (EScript == nullptr)
	//		continue;
	//	else
	//	{
	//		// 적 점 UI생성
	//		CGameObject* EnemyDot = new CGameObject;
	//		EnemyDot->SetName(L"EnemyDot");
	//		EnemyDot->AddComponent(new CUI);
	//		EnemyDot->AddComponent(new CUIRender);
	//
	//		// 적은 빨간색 점
	//		EnemyDot->UI()->SetColor(Vec4(1.f, 0.f, 0.f, 1.f));
	//		EnemyDot->UI()->SetRectSize(Vec2(6.f, 6.f));
	//
	//		// 미니맵 UI에 자식으로 추가
	//		GetOwner()->AddChild(EnemyDot);
	//
	//		// 적 점 저장
	//		m_EnemyDots.push_back({ EnemyObj, EnemyDot });
	//	}
	//}

	// 플레이어 방향 삼각형 UI 생성
	if (!m_PlayerArrow)
	{
		m_PlayerArrow = new CGameObject;
		m_PlayerArrow->SetName(L"PlayerMinimapUIArrow");
		m_PlayerArrow->AddComponent(new CUI);
		m_PlayerArrow->AddComponent(new CUIRender);

		m_PlayerArrow->UIRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"TriangleMesh"));
		m_PlayerArrow->UIRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UIMtrl"), 0);

		m_PlayerArrow->UI()->SetColor(Vec4(0.f, 1.f, 0.f, 0.2f));
		m_PlayerArrow->UI()->SetRectSize(Vec2(40.f, 80.f));
		m_PlayerArrow->UI()->SetRectPos(Vec2(0.f, 0.f));

		GetOwner()->AddChild(m_PlayerArrow);
	}
}

void MinimapUIScript::Tick()
{
	// 1. 플레이어는 무조건 Minimap의 중앙
	UpdatePlayerDot();

	// 2. 적은 빨간점으로 추적...이지만fps게임에서 적의 위치를 표기하는게맞나?
	//UpdateEnemyDots();

	// 3. 죽은 적 위치 업데이트
	CheckForNewDeadEnemies();
	UpdateDeadEnemies();
}

void MinimapUIScript::UpdatePlayerDot()
{
	if (!m_PlayerDot || !m_Player)
		return;

	// 플레이어는 항상 미니맵 중앙에 위치
	m_PlayerDot->UI()->SetRectPos(Vec2(0.f, 0.f));

	// 시야를 플레이어 방향으로 회전
	Vec3 playerRotation = m_Player->Transform()->GetRelativeRotation();
	float yRotation = playerRotation.y;
	float radians = yRotation * (XM_PI / 180.f);
	m_PlayerArrow->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, -yRotation));

	// 플레이어에서 조금 떨어진 위치에 배치
	float arrowDistance = 35.f;
	Vec2 arrowPos;
	arrowPos.x = -sin(radians) * arrowDistance;
	arrowPos.y = -cos(radians) * arrowDistance;

	m_PlayerArrow->UI()->SetRectPos(arrowPos);
}

void MinimapUIScript::UpdateEnemyDots()
{
	if (!m_Player)
		return;

	CGameObject* minimapCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MinimapCamera");

	// 미니맵 UI 크기와 플레이어 위치
	Vec3 playerPos = m_Player->Transform()->GetRelativePos();
	Vec2 minimapSize = UI()->GetRectPos();

	// 카메라의 View-Projection 행렬
	Matrix viewMatrix = minimapCamera->Camera()->GetViewMat();
	Matrix projMatrix = minimapCamera->Camera()->GetProjMat();
	Matrix viewProjMatrix = viewMatrix * projMatrix;

	// World좌표->미니맵카메라 좌표->NDC좌표->UI좌표변환
	for (auto& EnemyObj : m_EnemyDots)
	{
		if (!EnemyObj.Enemy || !EnemyObj.Dot)
			continue;

		Vec3 EnemyPos = EnemyObj.Enemy->Transform()->GetRelativePos();

		// World 좌표를 카메라 NDC좌표계로 변환
		Vec4 worldPos = Vec4(EnemyPos.x, EnemyPos.y, EnemyPos.z, 1.0f);
		Vec4 screenPos = XMVector4Transform(worldPos, viewProjMatrix);

		// NDC 좌표로 변환
		// 근데 어차피 직교투영이니까 해줄 필요없었음...
		//if (screenPos.w != 0.0f)
		//{
		//	screenPos.x /= screenPos.w;
		//	screenPos.y /= screenPos.w;
		//}

		// NDC를 UI 좌표로 변환
		Vec2 minimapSize = Vec2(300.f, 300.f);
		Vec2 uiPos;
		uiPos.x = screenPos.x * (minimapSize.x / 2.f);
		uiPos.y = (screenPos.y * (minimapSize.y / 2.f));

		// 화면 범위 내에 있는지 확인
		if (abs(screenPos.x) <= 1.0f && abs(screenPos.y) <= 1.0f)
		{
			EnemyObj.Dot->UI()->SetRectPos(uiPos);
			SetObjectActive(EnemyObj.Dot, true);
		}
		else
		{
			SetObjectActive(EnemyObj.Dot, false);
		}
	}
}

void MinimapUIScript::CheckForNewDeadEnemies()
{
	// 적 레이어(7번) 검사
	CLevel* currentLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	CLayer* enemyLayer = currentLevel->GetLayer(7);

	const vector<CGameObject*>& Enemies = enemyLayer->GetParentObjects();

	for (CGameObject* enemy : Enemies)
	{
		if (!enemy)
		{
			continue;
		}
		// 오브젝트 ID 가져오기
		UINT enemyID = enemy->GetObjectID();

		// EnemyController 컴포넌트 확인
		EnemyController* enemyController = static_cast<EnemyController*>(GetScriptWithType(enemy, SCRIPT_TYPE::ENEMYCONTROLLER));

		if (!enemyController)
		{
			continue;
		}

		// Death 상태이고 아직 처리되지 않은 적인지 ID로 확인
		if (enemyController->GetState() == Enemy_State::Death && m_ProcessedDeadEnemyIDs.find(enemyID) == m_ProcessedDeadEnemyIDs.end())
		{
			// 새로 죽은 적 위치 저장
			Vec3 deathPos = enemy->Transform()->GetRelativePos();

			// 죽은 적 UI 생성
			CGameObject* deadEnemyDot = new CGameObject;
			deadEnemyDot->SetName(L"DeadEnemyDot");
			deadEnemyDot->AddComponent(new CUI);
			deadEnemyDot->AddComponent(new CUIRender);

			// 죽은 적은 어두운 빨간색 표시
			deadEnemyDot->UI()->SetColor(Vec4(0.5f, 0.5f, 0.5f, 0.6f));
			deadEnemyDot->UI()->SetRectSize(Vec2(8.f, 8.f));

			GetOwner()->AddChild(deadEnemyDot);

			// 죽은 적 정보 추가
			DeadEnemyInfo deadInfo;
			deadInfo.WorldPos = deathPos;
			deadInfo.Dot = deadEnemyDot;
			deadInfo.EnemyID = enemyID;  // 원본 적 ID 저장

			m_DeadEnemies.push_back(deadInfo);

			// 처리 완료 표시 (중복 방지)
			m_ProcessedDeadEnemyIDs.insert(enemyID);
		}
	}
}

void MinimapUIScript::UpdateDeadEnemies()
{
	if (!m_Player)
	{
		return;
	}

	CGameObject* minimapCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MinimapCamera");
	if (!minimapCamera)
	{
		return;
	}

	Matrix viewMatrix = minimapCamera->Camera()->GetViewMat();
	Matrix projMatrix = minimapCamera->Camera()->GetProjMat();
	Matrix viewProjMatrix = viewMatrix * projMatrix;

	// 죽은 적들 업데이트
	for (int i = static_cast<int>(m_DeadEnemies.size()) - 1; i >= 0; --i)
	{
		DeadEnemyInfo& deadEnemy = m_DeadEnemies[i];

		// 위치 업데이트
		Vec4 worldPos = Vec4(deadEnemy.WorldPos.x, deadEnemy.WorldPos.y, deadEnemy.WorldPos.z, 1.0f);
		Vec4 screenPos = XMVector4Transform(worldPos, viewProjMatrix);

		Vec2 minimapSize = Vec2(300.f, 300.f);
		Vec2 uiPos;
		uiPos.x = screenPos.x * (minimapSize.x / 2.f);
		uiPos.y = screenPos.y * (minimapSize.y / 2.f);

		// 미니맵 범위 내에 있는지 확인
		if (abs(screenPos.x) <= 1.0f && abs(screenPos.y) <= 1.0f)
		{
			deadEnemy.Dot->UI()->SetRectPos(uiPos);
			SetObjectActive(deadEnemy.Dot, true);
		}
		else
		{
			SetObjectActive(deadEnemy.Dot, false);
		}
	}
}

void MinimapUIScript::SaveComponent(FILE* _File)
{
}

void MinimapUIScript::LoadComponent(FILE* _File)
{
}

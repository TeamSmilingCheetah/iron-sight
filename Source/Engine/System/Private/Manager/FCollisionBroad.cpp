#include "pch.h"
#include "Engine/System/Public/Manager/FCollisionManager.h"

#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"

/**************************/
/** Broad Phase Function **/
/**************************/

void FCollisionManager::BroadPhase()
{
	// Level Load Check
	auto* CurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (!CurrentLevel)
	{
		LOG_INFO("[Collision][Broad] No Level Found");
		return;
	}

	// Object Vector에 Object 전체 추가
	vector<CGameObject*> AllObjects;
	CurrentLevel->GetAllActiveObjectsInLevel(AllObjects);

	// 중복 배제를 위한 Set 추가
	unordered_set<ULONGLONG> CandidateSet;

	for (CGameObject* Object : AllObjects)
	{
		// 충돌 후보 선별
		vector<CGameObject*> Candidates;
		QueryBVH(MBVHRootNode, Object, Candidates);

		UINT ID_A = Object->GetID();

		for (CGameObject* Candidate : Candidates)
		{
			// Layer 조건 우선 배제
			if (MLayerCollisionMatrix[Object->GetLayerIdx()] & (1 << Candidate->GetLayerIdx()))
			{
				UINT ID_B = Candidate->GetID();

				// 고유 충돌 ID 생성
				if (ID_A > ID_B)
				{
					std::swap(ID_A, ID_B);
				}

				// 중복되지 않은 경우에만 세부 충돌 판정
				if (CandidateSet.insert(COLLISION_ID(ID_A, ID_B).ID).second)
				{
					AddCandidate(Object, Candidate);
				}
			}
		}
	}
}

/**
 * @brief 충돌 가능성이 보이는 오브젝트 쌍을 후보군 벡터에 올리는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void FCollisionManager::AddCandidate(const CGameObject* PLeftObject, const CGameObject* PRightObject)
{
	MCandidatePairVector.push_back({PLeftObject, PRightObject});
}

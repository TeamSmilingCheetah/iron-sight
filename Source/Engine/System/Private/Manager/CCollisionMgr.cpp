#include "pch.h"
#include "System/Public/Manager/CCollisionMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Component/Physics/CCollider2D.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"
#include "System/Public/Manager/CLevelMgr.h"

CCollisionMgr::CCollisionMgr()
{

}

CCollisionMgr::~CCollisionMgr()
{

}

// 충돌 체크가 필요한 레이어인지 확인
void CCollisionMgr::Tick()
{
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
		return;


	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			// 1이란 값이 없는 레이어면 확인 하지않음
			if (!(m_Matrix[Row] & (1 << Col)))
				continue;

			// Row, Col 두 레이어 충돌체크를 해야한다.
			CollisionBtwLayer(Row, Col);
		}
	}


	// LandScape의 충돌 작업(Ray만 판별, 3D는 이미 먼저 처리됨, 복수타겟 레이도 여기서 처리됨)
	LandCheak();


	// Ray충돌 작업(단일 타겟만 여기서 추가 연산됨)
	RayOverlapCheak();
	

	// 정보 초기화
	m_LandObject.clear();
	m_RayColInfo.clear();
}

// 각 레이어에 들어있는 오브젝트들을 확인
void CCollisionMgr::CollisionBtwLayer(UINT _Left, UINT _Right)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	const vector<CGameObject*>& vecLeft = pCurLevel->GetLayer(_Left)->GetObjects();
	const vector<CGameObject*>& vecRight = pCurLevel->GetLayer(_Right)->GetObjects();

	// 충돌 검사를 하려는 두 레이어가 다른 레이어인 경우
	if (_Left != _Right)
	{
		for (size_t i = 0; i < vecLeft.size(); ++i)
		{
			// 2D 충돌체 검사
			if (vecLeft[i]->Collider2D())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider2D())
						CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
				}
			}

			// 3D 충돌체 검사
			if (vecLeft[i]->Collider3D())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					// 3D간 충돌의 경우
					if (vecRight[j]->Collider3D())
						CollisionBtwCollider3D(vecLeft[i]->Collider3D(), vecRight[j]->Collider3D());

					// LandScape와 충돌의 경우
					if (vecRight[j]->LandScape())
					{
						CollisionBtwLandScape3D(vecLeft[i]->Collider3D(), vecRight[j]->LandScape());
					}

					// Ray와 충돌의 경우
					if (vecRight[j]->ColliderRay())
					{
						CollisionBtwColliderRay(vecRight[j]->ColliderRay(), vecLeft[i]->Collider3D());
					}
				}
			}

			// LandScape 검사
			if (vecLeft[i]->LandScape())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					// 3D간 충돌의 경우
					if (vecRight[j]->Collider3D())
						CollisionBtwLandScape3D(vecRight[j]->Collider3D(), vecLeft[i]->LandScape());

					// Ray와 충돌의 경우
					if (vecRight[j]->ColliderRay())
						CollisionBtwLandScapeRay(vecRight[j]->ColliderRay(), vecLeft[i]->LandScape());
				}
			}

			// RayCast 검사
			if (vecLeft[i]->ColliderRay())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider3D())
						CollisionBtwColliderRay(vecLeft[i]->ColliderRay(), vecRight[j]->Collider3D());

					if (vecRight[j]->LandScape())
						CollisionBtwLandScapeRay(vecLeft[i]->ColliderRay(), vecRight[j]->LandScape());
				}

			}
		}
	}

	// 두 레이어가 동일한 경우
	else
	{
		for (size_t i = 0; i < vecLeft.size(); ++i)
		{
			// 2D 충돌체 검사
			if (vecLeft[i]->Collider2D())
			{
				// 두 레이어가 동일한경우면 전에 검사한 오브젝트를 검사하는 경우를 제외해야한다.
				// 그러니 i +1 부터 시작하도록 한다.
				for (size_t j = i + 1; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider2D())
						CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
				}
			}

			// 3D 충돌체 검사
			if (vecLeft[i]->Collider3D())
			{
				for (size_t j = i + 1; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider3D())
						CollisionBtwCollider3D(vecLeft[i]->Collider3D(), vecRight[j]->Collider3D());

					// LandScape와 충돌의 경우
					if (vecRight[j]->LandScape())
					{
						CollisionBtwLandScape3D(vecLeft[i]->Collider3D(), vecRight[j]->LandScape());
					}

					// Ray와 충돌의 경우
					if (vecRight[j]->ColliderRay())
					{
						CollisionBtwColliderRay(vecRight[j]->ColliderRay(), vecLeft[i]->Collider3D());
					}
				}
			}

			// LandScape 검사
			if (vecLeft[i]->LandScape())
			{
				for (size_t j = i + 1; j < vecRight.size(); ++j)
				{
					// 3D간 충돌의 경우
					if (vecRight[j]->Collider3D())
						CollisionBtwLandScape3D(vecRight[j]->Collider3D(), vecLeft[i]->LandScape());

					// Ray와 충돌의 경우
					if (vecRight[j]->ColliderRay())
						CollisionBtwLandScapeRay(vecRight[j]->ColliderRay(), vecLeft[i]->LandScape());
				}
			}

			// RayCast검사
			if (vecLeft[i]->ColliderRay())
			{
				for (size_t j = i + 1; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider3D())
						CollisionBtwColliderRay(vecLeft[i]->ColliderRay(), vecRight[j]->Collider3D());

					// LandScape와 충돌의 경우
					if (vecRight[j]->LandScape())
					{
						CollisionBtwLandScapeRay(vecLeft[i]->ColliderRay(), vecRight[j]->LandScape());
					}
				}

			}
		}
	}

}

// Collider 컴포넌트 충돌체크 후 알맞게 호출
void CCollisionMgr::CollisionBtwCollider2D(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	// 한번도 등록된 적이 없었다.
	if (iter == m_ColInfo.end())
	{
		// 충돌 조합 등록
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// 한쪽이 Dead상태
	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();

	// 현재 겹쳐있다.
	if (IsCollision(_LeftCol, _RightCol))
	{

		//충돌중이다.
		if (iter->second)
		{
			// 둘중 하나가 곧 삭제 예정이다.
			if (IsDead)
			{

			}
			else
			{
				_LeftCol->Overlap(_RightCol);
				_RightCol->Overlap(_LeftCol);
			}

		}
		// 이전에는 떨어져 있었다.
		else
		{
			// 둘중 하나가 dead상태가 아니다
			if (!IsDead)
			{
				_LeftCol->BeginOverlap(_RightCol);
				_RightCol->BeginOverlap(_LeftCol);
			}
			iter->second = true;
		}

	}

	// 현재 떨어져 있다.
	else
	{
		if (iter->second == true)
		{
			_LeftCol->EndOverlap(_RightCol);
			_RightCol->EndOverlap(_LeftCol);
			iter->second = false;
		}

	}

}

void CCollisionMgr::CollisionBtwCollider3D(CCollider3D* _LeftCol, CCollider3D* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
	bool IsDeactive = _LeftCol->GetState() == DEACTIVE || _RightCol->GetState() == DEACTIVE;

	if (IsCollision3D(_LeftCol, _RightCol))
	{
		if (iter->second)
		{
			// 둘중 하나가 곧 삭제 예정이다.
			if (IsDead || IsDeactive)
			{
				_LeftCol->EndOverlap(_RightCol);
				_RightCol->EndOverlap(_LeftCol);
				iter->second = false;
			}
			else
			{
				_LeftCol->Overlap(_RightCol);
				_RightCol->Overlap(_LeftCol);
			}
		}
		else
		{
			if (!IsDead && !IsDeactive)
			{
				_LeftCol->BeginOverlap(_RightCol);
				_RightCol->BeginOverlap(_LeftCol);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			_LeftCol->EndOverlap(_RightCol);
			_RightCol->EndOverlap(_LeftCol);
			iter->second = false;
		}
	}
}

void CCollisionMgr::CollisionBtwLandScape3D(CCollider3D* _LeftCol, CLandScape* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
	bool IsDeactive = _LeftCol->GetState() == DEACTIVE;

	if (IsCollision3DLand(_LeftCol, _RightCol))
	{
		if (iter->second)
		{
			// 둘중 하나가 곧 삭제 예정이다.
			if (IsDead || IsDeactive)
			{
				_LeftCol->EndOverlap(_RightCol);
				iter->second = false;
			}
			else
			{
				_LeftCol->Overlap(_RightCol);
			}
		}
		else
		{
			if (!IsDead && !IsDeactive)
			{
				_LeftCol->BeginOverlap(_RightCol);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			_LeftCol->EndOverlap(_RightCol);
			iter->second = false;
		}
	}
}

void CCollisionMgr::CollisionBtwColliderRay(CColliderRay* _LeftCol, CCollider3D* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
	bool IsDeactive = _LeftCol->GetState() == DEACTIVE || _RightCol->GetState() == DEACTIVE;

	// 단일 타겟이면 Ray용 연산을 위해 연산 후 저장
	if (!(_LeftCol->IsTargetAllMode()))
	{
		if(!(IsDead) || !(IsDeactive))
			IsCollisionRay(_LeftCol, _RightCol);
	}
	else
	{

		if (IsCollisionRay(_LeftCol, _RightCol))
		{
			if (iter->second)
			{
				// 둘중 하나가 곧 삭제 예정이다.
				if (IsDead || IsDeactive)
				{
					_LeftCol->EndOverlap(_RightCol);
					_RightCol->EndOverlap(_LeftCol);
					iter->second = false;
				}
				else
				{
					_LeftCol->Overlap(_RightCol);
					_RightCol->Overlap(_LeftCol);
				}
			}
			else
			{
				if (!IsDead && !IsDeactive)
				{
					_LeftCol->BeginOverlap(_RightCol);
					_RightCol->BeginOverlap(_LeftCol);
					iter->second = true;
				}
			}
		}
		else
		{
			if (iter->second)
			{
				_LeftCol->EndOverlap(_RightCol);
				_RightCol->EndOverlap(_LeftCol);
				iter->second = false;
			}
		}
	}

}

void CCollisionMgr::CollisionBtwLandScapeRay(CColliderRay* _LeftCol, CLandScape* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// LandScape에 Ray정보 등록
	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
	bool IsDeactive = _LeftCol->GetState() == DEACTIVE;

	if(!(IsDead) && !(IsDeactive))
		IsCollisionRayLand(_LeftCol, _RightCol);


	//// 단일 타겟이면 Ray용 연산만 진행
	//if (!(_LeftCol->IsTargetAllMode()))
	//{
	//	IsCollisionRayLand(_LeftCol, _RightCol);
	//}
	//else
	//{
	//	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
	//	bool IsDeactive = _LeftCol->GetState() == DEACTIVE;
	//
	//	if (IsCollisionRayLand(_LeftCol, _RightCol))
	//	{
	//		if (iter->second)
	//		{
	//			// 둘중 하나가 곧 삭제 예정이다.
	//			if (IsDead || IsDeactive)
	//			{
	//				_LeftCol->EndOverlap(_RightCol);
	//				iter->second = false;
	//			}
	//			else
	//			{
	//				_LeftCol->Overlap(_RightCol);
	//			}
	//		}
	//		else
	//		{
	//			if (!IsDead)
	//			{
	//				_LeftCol->BeginOverlap(_RightCol);
	//			}
	//			iter->second = true;
	//		}
	//	}
	//	else
	//	{
	//		if (iter->second)
	//		{
	//			_LeftCol->EndOverlap(_RightCol);
	//			iter->second = false;
	//		}
	//	}
	//}
}

void CCollisionMgr::LandCheak()
{
	// LandScape에서 ray연산호출 이후 값 보관
	set<CLandScape*>::iterator Lnaditer = m_LandObject.begin();
	for (; Lnaditer != m_LandObject.end(); ++Lnaditer)
	{
		CLandScape* pLandscape = *Lnaditer;

		// 연산 이후 데이터 받아오기
		vector<tRayCollision>& RayCol = pLandscape->Collidercalcul();
		CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

		// 연산된 데이터 처리
		for (size_t i = 0; i < RayCol.size(); ++i)
		{
			// 레이 콜라이더 찾기
			CColliderRay* Ray = (CColliderRay*)RayCol[i].RayObj;

			COLLIDER_ID id = {};
			id.Left = Ray->GetID();
			id.Right = pLandscape->GetID();

			// 다중타겟모드시 overlap처리
			if (Ray->IsTargetAllMode())
			{

				map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

				if (iter == m_ColInfo.end())
				{
					m_ColInfo.insert(make_pair(id.ID, false));
					iter = m_ColInfo.find(id.ID);
				}

				bool IsDead = Ray->GetOwner()->IsDead() || pLandscape->GetOwner()->IsDead();
				bool IsDeactive = Ray->GetState() == DEACTIVE;

				if (RayCol[i].Success == 1)
				{
					if (iter->second)
					{
						// 둘중 하나가 곧 삭제 예정이다.
						if (IsDead || IsDeactive)
						{
							Ray->EndOverlap(pLandscape);
							iter->second = false;
						}
						else
						{
							Ray->Overlap(pLandscape);
						}
					}
					else
					{
						if (!IsDead)
						{
							Ray->BeginOverlap(pLandscape);
						}
						iter->second = true;
					}
				}
				else
				{
					if (iter->second)
					{
						Ray->EndOverlap(pLandscape);
						iter->second = false;
					}
				}
				Ray->SetRayTargetLength(RayCol[i].Distance);
			}
			// 단일 타겟 모드 시
			else
			{
				m_RayColInfo.insert(Ray);

				if (RayCol[i].Success == 1)
				{
					// 단일타겟모드시 ray콜라이더에 전용 데이터 삽입
					Ray->UpdateRayColInfo(pLandscape->GetOwner(), RayCol[i].Distance);
				}
			}

		}

	}

}

void CCollisionMgr::RayOverlapCheak()
{
	// 비어잇을경우 정지
	if (m_RayColInfo.empty())
		return;

	set<CColliderRay*>::iterator setiter = m_RayColInfo.begin();
	
	// 거리순으로 정렬된 상태에서 각 객체의 충돌 상태 확인 및 처리
	for (; setiter != m_RayColInfo.end(); ++setiter)
	{
		CColliderRay* pRay = *setiter;

		// 단일충돌용 데이터 가져오기
		RAYCOLLIDERDATA& data = (*setiter)->GetTargetInfoRef();

		// 충돌되는 오브젝트가 있나 확인
		if (data.HitObject != nullptr)
		{
			// 현재 충돌 오브젝트부터 판단
			COLLIDER_ID hitid = {};
			hitid.Left = (*setiter)->GetID();
			hitid.Right = data.HitObject->GetID();

			map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(hitid.ID);

			if (iter == m_ColInfo.end())
			{
				m_ColInfo.insert(make_pair(hitid.ID, false));
				iter = m_ColInfo.find(hitid.ID);
			}



			// 콜라이더 가져오기
			CGameObject* RightObject = data.HitObject;

			bool IsDead = pRay->GetOwner()->IsDead() || RightObject->IsDead();
			bool IsDeactive = pRay->GetState() == DEACTIVE;

			// LandScape인지 3D인지 구분해서 처리
			if (RightObject->Collider3D())	// 3D타입
			{
				CCollider3D* p3DCol = RightObject->Collider3D();

				IsDeactive = p3DCol->GetState() == DEACTIVE;

				if (iter->second)
				{
					// 둘중 하나가 곧 삭제 예정이다.
					if (IsDead || IsDeactive)
					{
						pRay->EndOverlap(p3DCol);
						p3DCol->EndOverlap(pRay);
						iter->second = false;
					}
					else
					{
						pRay->Overlap(p3DCol);
						p3DCol->Overlap(pRay);
					}
				}
				else
				{
					if (!IsDead && !IsDeactive)
					{
						pRay->BeginOverlap(p3DCol);
						p3DCol->BeginOverlap(pRay);
						iter->second = true;
					}
				}


			}
			else if (RightObject->LandScape())// LANDSCAPE 타입
			{
				CLandScape* pLandCol = RightObject->LandScape();

				if (iter->second)
				{
					// 둘중 하나가 곧 삭제 예정이다.
					if (IsDead || IsDeactive)
					{
						pRay->EndOverlap(pLandCol);
						iter->second = false;
					}
					else
					{
						pRay->Overlap(pLandCol);
					}
				}
				else
				{
					if (!IsDead && !IsDeactive)
					{
						pRay->BeginOverlap(pLandCol);
						iter->second = true;
					}
				}
			}
		}
		
	

		// Prev오브젝트 판단
		// 이전오브젝트와 현재 오브젝트가 같다 = 위에서 overlap처리되어 해당 사항 넘김
		if (data.HitObject == data.PrevObject || data.PrevObject == nullptr)
			continue;

		COLLIDER_ID Previd = {};
		Previd.Left = (*setiter)->GetID();
		Previd.Right = data.PrevObject->GetID();

		map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(Previd.ID);

		// 등록 하는게 없는 이유는 이전에 충돌도 안햇는데 이전 충돌했다고 보고 해버리면 그것대로 문제
		iter = m_ColInfo.find(Previd.ID);

		if (iter == m_ColInfo.end())
			assert(true);


		CGameObject* RightObject = data.PrevObject;

		bool IsDead = pRay->GetOwner()->IsDead() || RightObject->IsDead();
		bool IsDeactive = pRay->GetState() == DEACTIVE;

		// LandScape인지 3D인지 구분해서 처리
		if (RightObject->Collider3D())	// 3D타입
		{
			CCollider3D* p3DCol = RightObject->Collider3D();

			IsDeactive = p3DCol->GetState() == DEACTIVE;

			// 서로 떨어진 것은 확정
			if (iter->second)
			{
				pRay->EndOverlap(p3DCol);
				p3DCol->EndOverlap(pRay);
				iter->second = false;
			}

		}
		else if (RightObject->LandScape())// LANDSCAPE 타입
		{
			CLandScape* pLandCol = RightObject->LandScape();

			// 서로 떨어진 것은 확정
			if (iter->second)
			{
				pRay->EndOverlap(pLandCol);
				iter->second = false;
			}
		}
	}
}

// 충돌 체크
bool CCollisionMgr::IsCollision(CCollider2D* _Left, CCollider2D* _Right)
{
	// 0 -- 1
	// | \  |
	// 3 -- 2 
	// Local Mesh 정점 Pos값
	static Vec3 arrRect[4] =
	{
		Vec3(-0.5f, 0.5f, 0.f),
		Vec3(0.5f, 0.5f, 0.f),
		Vec3(0.5f, -0.5f, 0.f),
		Vec3(-0.5f, -0.5f, 0.f)
	};

	Matrix matColLeft = _Left->GetColliderWorldMat();
	Matrix matColRight = _Right->GetColliderWorldMat();

	// 투영축 구하기, 투영축 == 투영을 시킬 대상
	Vec3 arrProj[4] = {};

	//충돌체 좌상단,우상단 좌표(투영축 0)
	arrProj[0] = XMVector3TransformCoord(arrRect[1], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
	//충돌체 좌상단,좌하단 좌표(투영축 1)
	arrProj[1] = XMVector3TransformCoord(arrRect[3], matColLeft) - XMVector3TransformCoord(arrRect[0], matColLeft);
	//오른쪽 충돌체 좌상단,우상단 좌표(투영축 2)
	arrProj[2] = XMVector3TransformCoord(arrRect[1], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);
	//오른쪽 충돌체 좌상단,좌하단 좌표(투영축 3)
	arrProj[3] = XMVector3TransformCoord(arrRect[3], matColRight) - XMVector3TransformCoord(arrRect[0], matColRight);

	// 월드 공간에서 두 충돌체의 중심을 이은 벡터
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);

	for (int i = 0; i < 4; ++i)
	{
		Vec3 vProj = arrProj[i];
		vProj.Normalize();	// 노말라이즈 해서 투영면의 길이를 1로 만든다.


		// 각도가 90도 이상이면 음수가 나오니 절대값을 해준다.
		float fCenter = fabs(vCenter.Dot(vProj));	// 센터끼리 이은길이
		float fDist = 0.f;
		for (int j = 0; j < 4; ++j)
		{
			fDist += fabs(vProj.Dot(arrProj[j]));
		}
		fDist /= 2.f;

		if (fDist < fCenter)
			return false;
	}


	return true;
}

bool CCollisionMgr::IsCollision3D(CCollider3D* _Left, CCollider3D* _Right)
{
	const float EPSILON = 0.0001f;

	// 로컬 큐브의 정점 위치 (8개의 정점)
	static Vec3 arrCube[8] =
	{
		Vec3(-0.5f, 0.5f, 0.5f),   // 전면 좌상단
		Vec3(0.5f, 0.5f, 0.5f),    // 전면 우상단
		Vec3(0.5f, -0.5f, 0.5f),   // 전면 우하단
		Vec3(-0.5f, -0.5f, 0.5f),  // 전면 좌하단
		Vec3(-0.5f, 0.5f, -0.5f),  // 후면 좌상단
		Vec3(0.5f, 0.5f, -0.5f),   // 후면 우상단
		Vec3(0.5f, -0.5f, -0.5f),  // 후면 우하단
		Vec3(-0.5f, -0.5f, -0.5f)  // 후면 좌하단
	};


	Matrix matColLeft = _Left->GetColliderWorldMat();
	Matrix matColRight = _Right->GetColliderWorldMat();

	Vec3 leftCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft);
	Vec3 rightCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);
	Vec3 centerToCenter = rightCenter - leftCenter;

	// 각 OBB의 월드 공간 꼭지점 계산
	Vec3 leftVertices[8];
	Vec3 rightVertices[8];
	for (int i = 0; i < 8; i++)
	{
		leftVertices[i] = XMVector3TransformCoord(arrCube[i], matColLeft);
		rightVertices[i] = XMVector3TransformCoord(arrCube[i], matColRight);
	}

	// 검사할 축들: 각 OBB의 면 법선(6개)과 모서리 방향의 cross product(9개)
	Vec3 axes[15];
	int axisCount = 0;

	// 각 OBB의 축(x,y,z) 계산
	Vec3 leftAxis[3] = {
		XMVector3TransformNormal(Vec3(1.0f, 0.0f, 0.0f), matColLeft),
		XMVector3TransformNormal(Vec3(0.0f, 1.0f, 0.0f), matColLeft),
		XMVector3TransformNormal(Vec3(0.0f, 0.0f, 1.0f), matColLeft)
	};

	Vec3 rightAxis[3] = {
		XMVector3TransformNormal(Vec3(1.0f, 0.0f, 0.0f), matColRight),
		XMVector3TransformNormal(Vec3(0.0f, 1.0f, 0.0f), matColRight),
		XMVector3TransformNormal(Vec3(0.0f, 0.0f, 1.0f), matColRight)
	};

	// OBB의 면 법선 추가
	for (int i = 0; i < 3; i++)
	{
		leftAxis[i].Normalize();
		rightAxis[i].Normalize();
		axes[axisCount++] = leftAxis[i];
		axes[axisCount++] = rightAxis[i];
	}

	// 모서리 방향의 cross product 추가
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Vec3 crossAxis = leftAxis[i].Cross(rightAxis[j]);
			if (crossAxis.Length() > EPSILON)  // 0벡터 제외
			{
				crossAxis.Normalize();
				axes[axisCount++] = crossAxis;
			}
		}
	}

	// 최소 침투 축과 침투 깊이 추적
	Vec3 minPenetrationAxis;
	float minPenetration = FLT_MAX;
	bool foundSeparatingAxis = false;

	// 각 축에 대해 분리축 테스트 수행
	for (int a = 0; a < axisCount; ++a)
	{
		float leftMin = FLT_MAX;
		float leftMax = -FLT_MAX;
		float rightMin = FLT_MAX;
		float rightMax = -FLT_MAX;

		// 왼쪽 OBB의 꼭지점들을 현재 축에 투영
		for (int i = 0; i < 8; i++)
		{
			float dot = leftVertices[i].Dot(axes[a]);
			leftMin = min(leftMin, dot);
			leftMax = max(leftMax, dot);
		}

		// 오른쪽 OBB의 꼭지점들을 현재 축에 투영
		for (int i = 0; i < 8; i++)
		{
			float dot = rightVertices[i].Dot(axes[a]);
			rightMin = min(rightMin, dot);
			rightMax = max(rightMax, dot);
		}

		// 분리축 테스트
		if (leftMin > rightMax + EPSILON || rightMin > leftMax + EPSILON)
			return false;

		// 침투 깊이 계산
		float penetration1 = rightMax - leftMin;
		float penetration2 = leftMax - rightMin;
		float penetration = min(penetration1, penetration2);

		// 최소 침투 업데이트
		if (penetration < minPenetration)
		{
			minPenetration = penetration;
			minPenetrationAxis = axes[a];
			foundSeparatingAxis = true;
		}
	}

	if (foundSeparatingAxis)
	{
		float dotWithCenterToCenter = minPenetrationAxis.Dot(centerToCenter);
		if (dotWithCenterToCenter < 0)
			minPenetrationAxis = -minPenetrationAxis;

		// 충돌 노말 설정
		_Left->SetHitNormal(-minPenetrationAxis);   // Left에서 Right로 향하는 방향
		_Right->SetHitNormal(minPenetrationAxis); // Right에서 Left로 향하는 방향
	}

	return true;
}

bool CCollisionMgr::IsCollisionRay(CColliderRay* _LeftCol, CCollider3D* _RightCol)
{
	// 로컬 큐브의 정점 위치 (8개의 정점)
	static Vec3 arrCube[8] =
	{
		Vec3(-0.5f, 0.5f, 0.5f),   // 전면 좌상단
		Vec3(0.5f, 0.5f, 0.5f),    // 전면 우상단
		Vec3(0.5f, -0.5f, 0.5f),   // 전면 우하단
		Vec3(-0.5f, -0.5f, 0.5f),  // 전면 좌하단
		Vec3(-0.5f, 0.5f, -0.5f),  // 후면 좌상단
		Vec3(0.5f, 0.5f, -0.5f),   // 후면 우상단
		Vec3(0.5f, -0.5f, -0.5f),  // 후면 우하단
		Vec3(-0.5f, -0.5f, -0.5f)  // 후면 좌하단
	};

	// 충돌체의 월드 행렬
	Matrix matWorld = _RightCol->GetColliderWorldMat();

	// 충돌체의 월드 공간 정점 계산
	Vec3 worldVerts[8];
	for (int i = 0; i < 8; ++i)
	{
		worldVerts[i] = XMVector3TransformCoord(arrCube[i], matWorld);
	}

	// 큐브의 12개 삼각형에 대해 레이-삼각형 교차 검사
	// 큐브의 각 면은 2개의 삼각형으로 구성 (총 6면 = 12 삼각형)

	// 삼각형 인덱스 정의 (12개 삼각형의 정점 인덱스)
	static int triangles[12][3] = {
		// 전면
		{0, 2, 1}, {0, 3, 2},
		// 우측면
		{1, 5, 6}, {1, 6, 2},
		// 후면
		{4, 5, 6}, {4, 6, 7},
		// 좌측면
		{0, 4, 7}, {0, 7, 3},
		// 상단
		{4, 5, 1}, {4, 1, 0},
		// 하단
		{3, 2, 6}, {3, 6, 7}
	};

	// 레이 정보 가져오기
	Vec3 rayPos = _LeftCol->GetRayFinalPos();
	Vec3 rayDir = _LeftCol->GetRayFinalDir();
	float rayMaxDist = _LeftCol->GetRayLength(); // 레이 최대 거리

	// 가장 가까운 충돌 거리 및 충돌 여부
	float closestDist = FLT_MAX;
	bool hasCollision = false;

	// 각 삼각형에 대해 레이 충돌 검사
	for (int i = 0; i < 12; ++i)
	{
		// 현재 삼각형의 정점 배열 구성
		Vec3* triVerts[3] =
		{
			&worldVerts[triangles[i][0]],
			&worldVerts[triangles[i][1]],
			&worldVerts[triangles[i][2]]
		};

		// 충돌 결과를 저장할 변수
		Vec3 crossPos = Vec3(0.f);
		float dist = 0;

		// IntersectsRay 함수로 레이-삼각형 충돌 검사
		if (IntersectsRay(triVerts, rayPos, rayDir, crossPos, dist))
		{
			// 충돌 거리가 최대 거리보다 작은지 확인
			if (dist >= 0.f && dist <= rayMaxDist)
			{
				if (dist >= 0.f && dist <= rayMaxDist && dist < closestDist)
				{
					closestDist = dist;
					hasCollision = true;
				}
			}
		}
	}

	// 충돌이 있을 경우에만 처리
	if (hasCollision)
	{
		// 모든타겟인지 단일타겟인지 확인
		if (_LeftCol->IsTargetAllMode())
		{
			_LeftCol->SetRayTargetLength(closestDist);
			return true;
		}
		else
		{
			// Update후 정리
			_LeftCol->UpdateRayColInfo(_RightCol->GetOwner(), closestDist);

			m_RayColInfo.insert(_LeftCol);

			return true;
		}
	}


	return false;
}

bool CCollisionMgr::IsCollision3DLand(CCollider3D* _LeftCol, CLandScape* _RightCol)
{
	// 오브젝트는 Transform기준 위치를 발로 잡는다.
	Vec3 ObjectPos = _LeftCol->Transform()->GetWorldPos();

	// 오브젝트 위치 기준으로 LandScape의 높이 측정
	Vec3 LandScapePos = _RightCol->GetWorldPosByLandScape(ObjectPos);

	// 말도 안되는 값이 들어오면 LandScpae위치를 벗어난 위치
	// y축 기준으로 오브젝트가 아래에 있다면 충돌
	if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
		return false;

	return true;
}

bool CCollisionMgr::IsCollisionRayLand(CColliderRay* _LeftCol, CLandScape* _RightCol)
{
	// Ray의 월드 위치, 방향정보를 받아온다.
	tRayCollision RayInfo;
	RayInfo.RayObj = _LeftCol;
	RayInfo.RayWorldPos = _LeftCol->GetRayFinalPos();
	RayInfo.RayDir = _LeftCol->GetRayFinalDir();
	RayInfo.RayLength = _LeftCol->GetRayLength();

	// LandScape에서 해당 Ray정보를 넘겨준다.(나중에 처리됨)
	_RightCol->AddRayCol(RayInfo);
	m_LandObject.insert(_RightCol);

	//if (calculInfo.Success && calculInfo.Distance > 0.f && calculInfo.Distance <= rayMaxDist)
	//{
	//	if (_LeftCol->IsTargetAllMode())
	//	{
	//		_LeftCol->SetRayTargetLength(calculInfo.Distance);
	//		return true;
	//	}
	//	else
	//	{
	//		_LeftCol->UpdateRayColInfo(_RightCol->GetOwner(), calculInfo.Distance);
	//		return true;
	//	}
	//}

	return false;
}

// 충돌레이어 등록
void CCollisionMgr::CollisionCheck(UINT _Left, UINT _Right)
{
	UINT Row = _Left;
	UINT Col = _Right;

	if (Col < Row)
	{
		Row = _Right;
		Col = _Left;
	}


	if (m_Matrix[Row] & (1 << Col))
	{
		m_Matrix[Row] &= ~(1 << Col);
	}
	else
	{
		m_Matrix[Row] |= (1 << Col);
	}
}

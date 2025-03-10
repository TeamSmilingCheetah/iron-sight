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
				// 레이 처리 로직
				if (vecLeft[i]->ColliderRay()->IsTargetAll())
				{
					// Target조건이 모든 타겟일 경우
					for (size_t j = 0; j < vecRight.size(); ++j)
					{
						if (vecRight[j]->Collider3D())
							CollisionBtwColliderRay(vecLeft[i]->ColliderRay(), vecRight[j]->Collider3D());

						if (vecRight[j]->LandScape())
							CollisionBtwLandScapeRay(vecLeft[i]->ColliderRay(), vecRight[j]->LandScape());
					}
				}
				else
				{
					// Ray가 타겟 하나만 판별하는 경우
					for (size_t j = 0; j < vecRight.size(); ++j)
					{

					}
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
				// Target조건이 하나가 아닐 경우
				if (vecLeft[i]->ColliderRay()->IsTargetAll())
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

				// Ray가 타겟 하나만 판별하는 경우 추가 연산 처리
				else
				{
					for (size_t j = i + 1; j < vecRight.size(); ++j)
					{
                          
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
			}
			iter->second = true;
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
			if (!IsDead)
			{
				_LeftCol->BeginOverlap(_RightCol);
			}
			iter->second = true;
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

void CCollisionMgr::CollisionBtwColliderRay(CColliderRay* _LeftCol, CCollider3D* _RightCol, bool _CalculOnly)
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

	// Ray용 연산만 진행
	if (_CalculOnly)
	{
		IsCollisionRay(_LeftCol, _RightCol);
	}
	else
	{
		bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
		bool IsDeactive = _LeftCol->GetState() == DEACTIVE || _RightCol->GetState() == DEACTIVE;

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
				if (!IsDead)
				{
					_LeftCol->BeginOverlap(_RightCol);
					_RightCol->BeginOverlap(_LeftCol);
				}
				iter->second = true;
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

void CCollisionMgr::CollisionBtwLandScapeRay(CColliderRay* _LeftCol, CLandScape* _RightCol, bool _CalculOnly)
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

	// Ray용 연산만 진행
	if (_CalculOnly)
	{
		IsCollisionRayLand(_LeftCol, _RightCol);
	}
	else
	{
		bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();
		bool IsDeactive = _LeftCol->GetState() == DEACTIVE;

		if (IsCollisionRayLand(_LeftCol, _RightCol))
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
				if (!IsDead)
				{
					_LeftCol->BeginOverlap(_RightCol);
				}
				iter->second = true;
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
}

void CCollisionMgr::RayOverlapCheak()
{
	//if (m_RayColInfo.empty())
	//	return;
	//
	//// 거리순으로 정렬된 상태에서 각 객체의 충돌 상태 확인 및 처리
	//for (size_t i = 0; i < m_RayColInfo.size(); ++i)
	//{
	//	CColliderRay* LeftCol = (CColliderRay*)(m_RayColInfo[i].ObjectLeft->GetComponent(COMPONENT_TYPE::COLLIDERRAY));
	//	CComponent* RightCol = (m_RayColInfo[i].ObjectRight->GetComponent(COMPONENT_TYPE::COLLIDER3D));
	//	if (RightCol == nullptr)
	//	{
	//		RightCol = m_RayColInfo[i].ObjectRight->GetComponent(COMPONENT_TYPE::LANDSCAPE);
	//	}
	//
	//	COLLIDER_ID id = {};
	//	id.Left = LeftCol->GetID();
	//	id.Right = RightCol->GetID();
	//
	//	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);
	//	bool IsDead = m_RayColInfo[i].ObjectLeft->IsDead() || m_RayColInfo[i].ObjectRight->IsDead();
	//	bool IsDeactive = LeftCol->GetState() == DEACTIVE;
	//
	//	// 가장 가까운 충돌이며 충돌 상태일 시 true
	//	bool isClosestAndCollided = (i == 0 && m_RayColInfo[i].Hit);
	//
	//	// LandScape인지 3D인지 구분해서 처리
	//	if (RightCol->GetType() == COMPONENT_TYPE::COLLIDER3D)
	//	{
	//		CCollider3D* RightCol3D = (CCollider3D*)RightCol;
	//
	//		// 가장 가까운 충돌 대상이고 실제 충돌했다면
	//		if (isClosestAndCollided)
	//		{
	//			// 이미 충돌 중인 상태
	//			if (iter->second)
	//			{
	//				if (IsDead || IsDeactive)
	//				{
	//					LeftCol->EndOverlap(RightCol3D);
	//					RightCol3D->EndOverlap(LeftCol);
	//					iter->second = false;
	//				}
	//				else
	//				{
	//					LeftCol->Overlap(RightCol3D);
	//					RightCol3D->Overlap(LeftCol);
	//				}
	//			}
	//			// 충돌중이 아닌 상태
	//			else
	//			{
	//				if (!IsDead)
	//				{
	//					LeftCol->BeginOverlap(RightCol3D);
	//					RightCol3D->BeginOverlap(LeftCol);
	//				}
	//				iter->second = true;
	//			}
	//		}
	//		// 가장 가까운 대상이 아니거나 충돌하지 않았지만 이전에 충돌 중이었다면
	//		// 거기다 이전에 충돌중이었을 시
	//		else if (iter->second)
	//		{
	//			LeftCol->EndOverlap(RightCol3D);
	//			RightCol3D->EndOverlap(LeftCol);
	//			iter->second = false;
	//		}
	//	}
	//	else // LANDSCAPE 타입
	//	{
	//		CLandScape* RightColLand = (CLandScape*)RightCol;
	//
	//		// 가장 가까운 충돌 대상이고 실제 충돌했다면
	//		if (isClosestAndCollided)
	//		{
	//			// 이미 충돌 중인 상태
	//			if (iter->second)
	//			{
	//				if (IsDead || IsDeactive)
	//				{
	//					LeftCol->EndOverlap(RightColLand);
	//					iter->second = false;
	//				}
	//				else
	//				{
	//					LeftCol->Overlap(RightColLand);
	//				}
	//			}
	//			// 충돌중이 아닌 상태
	//			else
	//			{
	//				if (!IsDead)
	//				{
	//					LeftCol->BeginOverlap(RightColLand);
	//				}
	//				iter->second = true;
	//			}
	//		}
	//		// 가장 가까운 대상이 아니거나 충돌하지 않았지만 이전에 충돌 중이었다면
	//		else if (iter->second)
	//		{
	//			LeftCol->EndOverlap(RightColLand);
	//			iter->second = false;
	//		}
	//	}
	//}
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

	// 각 OBB의 월드 공간 꼭지점 계산
	Vec3 leftVertices[8];
	Vec3 rightVertices[8];
	for (int i = 0; i < 8; i++)
	{
		leftVertices[i] = XMVector3TransformCoord(arrCube[i], matColLeft);
		rightVertices[i] = XMVector3TransformCoord(arrCube[i], matColRight);
	}

	// 검사할 축들: 각 OBB의 면 법선(6개)과 모서리 방향의 cross product(9개)
	vector<Vec3> axes;

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
		axes.push_back(leftAxis[i]);
		axes.push_back(rightAxis[i]);
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
				axes.push_back(crossAxis);
			}
		}
	}

	// 각 축에 대해 분리축 테스트 수행
	vector<Vec3>::iterator iter = axes.begin();
	for (; iter != axes.end(); ++iter)
	{
		float leftMin = FLT_MAX;
		float leftMax = -FLT_MAX;
		float rightMin = FLT_MAX;
		float rightMax = -FLT_MAX;

		// 왼쪽 OBB의 꼭지점들을 현재 축에 투영
		for (int i = 0; i < 8; i++)
		{
			float dot = leftVertices[i].Dot(*iter);
			leftMin = min(leftMin, dot);
			leftMax = max(leftMax, dot);
		}

		// 오른쪽 OBB의 꼭지점들을 현재 축에 투영
		for (int i = 0; i < 8; i++)
		{
			float dot = rightVertices[i].Dot(*iter);
			rightMin = min(rightMin, dot);
			rightMax = max(rightMax, dot);
		}

		// 분리축 테스트
		if (leftMin > rightMax + EPSILON || rightMin > leftMax + EPSILON)
			return false;
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
				_LeftCol->SetRayTargetLength(dist);
				// 조건이 타겟 하나만 일 시
				if (_LeftCol->IsTargetAll())
				{
					return true;
				}
				else
				{
					return true;
				}
			}
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
	tRay RayInfo;
	RayInfo.vStart = _LeftCol->GetRayFinalPos();
	RayInfo.vDir = _LeftCol->GetRayFinalDir();
	float rayMaxDist = _LeftCol->GetRayLength();

	// LandScape에서 해당 Ray정보를 넘겨 CS연산 후 받아온다.
	tRaycastOut calculInfo = _RightCol->ColliderRaycasting(RayInfo);

	// 충돌일 시 처리
	if (calculInfo.Success && calculInfo.Distance > 0.f && calculInfo.Distance <= rayMaxDist)
	{
		_LeftCol->SetRayTargetLength(calculInfo.Distance);

		// 추후 조건 추가
		if (_LeftCol->IsTargetAll())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

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

#include "pch.h"
#include "System/Public/Manager/CCollisionMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Actor/CLevel.h"
#include "Runtime/Public/Component/Physics/CCollider2D.h"
#include "Runtime/Public/Component/Physics/CCollider3D.h"
#include "Runtime/Public/Component/Physics/CColliderRay.h"
#include "System/Public/Manager/CLevelMgr.h"

CCollisionMgr::CCollisionMgr()
	: m_Matrix{}
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::Tick()
{
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
		return;


	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			if (!(m_Matrix[Row] & (1 << Col)))
				continue;

			// Row , Col 두 레이어가 충돌체크를 해야한다.
			CollisionBtwLayer(Row, Col);
		}
	}
}

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
					if (vecRight[j]->Collider3D())
						CollisionBtwCollider3D(vecLeft[i]->Collider3D(), vecRight[j]->Collider3D());
				}
			}

			// RayCast검사
			if (vecLeft[i]->ColliderRay())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider3D())
						CollisionBtwColliderRay(vecLeft[i]->ColliderRay(), vecRight[j]->Collider3D());
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
				}
			}

			// RayCast검사
			if (vecLeft[i]->ColliderRay())
			{
				for (size_t j = 0; j < vecRight.size(); ++j)
				{
					if (vecRight[j]->Collider3D())
						CollisionBtwColliderRay(vecLeft[i]->ColliderRay(), vecRight[j]->Collider3D());
				}
			}
		}
	}

}

void CCollisionMgr::CollisionBtwCollider2D(CCollider2D* _LeftCol, CCollider2D* _RightCol)
{
	COLLIDER_ID id = {};
	id.Left = _LeftCol->GetID();
	id.Right = _RightCol->GetID();

	auto iter = m_ColInfo.find(id.ID);

	// 한번도 등록된 적이 없었다.
	if (iter == m_ColInfo.end())
	{
		// 충돌 조합 등록
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// 두 충돌체중 하나라도 Dead 상태인지 아닌지
	bool IsDead = _LeftCol->GetOwner()->IsDead() || _RightCol->GetOwner()->IsDead();

	// 현재 겹쳐있다.
	if (IsCollision(_LeftCol, _RightCol))
	{
		// 이전에도 겹쳐있었다.
		if (iter->second)
		{
			// 둘중 하나 이상이 곧 삭제 예정이다.
			if (IsDead)
			{
				_LeftCol->EndOverlap(_RightCol);
				_RightCol->EndOverlap(_LeftCol);
			}
			else
			{
				// 충돌중이다.
				_LeftCol->Overlap(_RightCol);
				_RightCol->Overlap(_LeftCol);
			}
		}

		// 이전에는 떨어져있었다.
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

	// 현재 떨어져있다.
	else
	{
		// 이전에는 겹쳐있었다.
		if (iter->second)
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

	if (IsCollision3D(_LeftCol, _RightCol))
	{
		if (iter->second)
		{
			if (!IsDead)
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

	if (IsCollisionRay(_LeftCol, _RightCol))
	{
		if (iter->second)
		{
			if (!IsDead)
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

bool CCollisionMgr::IsCollision(CCollider2D* _Left, CCollider2D* _Right)
{
	// 0 -- 1
	// | \  |
	// 3 -- 2
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
	arrProj[0] = XMVector3TransformCoord(arrRect[1], matColLeft) - XMVector3TransformCoord(
		arrRect[0], matColLeft);
	arrProj[1] = XMVector3TransformCoord(arrRect[3], matColLeft) - XMVector3TransformCoord(
		arrRect[0], matColLeft);
	arrProj[2] = XMVector3TransformCoord(arrRect[1], matColRight) - XMVector3TransformCoord(
		arrRect[0], matColRight);
	arrProj[3] = XMVector3TransformCoord(arrRect[3], matColRight) - XMVector3TransformCoord(
		arrRect[0], matColRight);

	// 월드공간에서 두 충돌체의 중심을 이은 벡터
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) -
		XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColRight);

	for (int i = 0; i < 4; ++i)
	{
		Vec3 vProj = arrProj[i];
		vProj.Normalize();

		float fCenter = fabs(vCenter.Dot(vProj));
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
		{0, 1, 2}, {0, 2, 3},
		// 우측면
		{1, 5, 6}, {1, 6, 2},
		// 후면
		{5, 4, 7}, {5, 7, 6},
		// 좌측면
		{4, 0, 3}, {4, 3, 7},
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
			// 0보다 작으면 ray의 뒤에 있으니 조건에서 제외
			if (dist >= 0.f && dist <= rayMaxDist)
			{
				return true;
			}
		}
	}

	return false;
}

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

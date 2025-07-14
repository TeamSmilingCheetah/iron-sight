#include "pch.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"

#include "Engine/System/Public/Manager/CCollisionMgr.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider2D.h"
#include "Engine/Runtime/Public/Component/Physics/CCollider3D.h"
#include "Engine/Runtime/Public/Component/Physics/CColliderRay.h"
#include "Engine/Runtime/Public/Component/Physics/CMeshCollider.h"
#include "Engine/Runtime/Public/Component/Rendering/CLandScape.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Shader/CMeshCollisionCS.h"

CCollisionMgr::CCollisionMgr() = default;

CCollisionMgr::~CCollisionMgr() = default;

/**
 * @brief 전체 충돌 처리 로직의 처리 함수
 */
void CCollisionMgr::Tick()
{
	// 0. Early Return
	if (!CLevelMgr::GetInst()->GetCurrentLevel())
		return;

	// 1. Layer 간의 충돌 처리
	ProcessCollisionMatrix();

	// 2. LandScape의 충돌 작업 (Ray만 판별, 3D는 이미 먼저 처리됨, 복수타겟 레이도 여기서 처리됨)
	LandCheak();

	// 3. 단일 타겟 Ray 충돌에 대한 추가 연산 처리
	RayOverlapCheak();

	// 4. Reset Information
	m_LandObject.clear();
	m_RayColInfo.clear();
}

/**
 * @brief 레이어 간의 충돌 설정을 변경하는 함수
 * 충돌 On의 경우 Off, Off인 경우 On으로 변경하도록 구현되어 있음
 */
void CCollisionMgr::ToggleLayerCollision(UINT PLeft, UINT PRight)
{
	UINT Row = PLeft;
	UINT Col = PRight;

	if (Col < Row)
	{
		Row = PRight;
		Col = PLeft;
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

/** Collision In Matrix **/

/**
 * @brief Layer 간의 충돌에 대해서 처리하는 함수
 */
void CCollisionMgr::ProcessCollisionMatrix()
{
	// Layer Matching
	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			if (m_Matrix[Row] & (1 << Col))
			{
				// 1. 단일 레이어 내의 충돌 처리
				if (Row == Col)
				{
					CollisionsInLayer(Row);
				}
				// 2. Matching된 Layer 간의 발생하는 충돌 처리
				else
				{
					CollisionBtwLayer(Row, Col);
				}
			}
		}
	}
}

/**
 * @brief 단일 레이어 내의 오브젝트 간의 충돌에 대해 처리하는 함수
 *
 * @param PLayerIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index
 */
void CCollisionMgr::CollisionsInLayer(UINT PLayerIndex)
{
	const vector<CGameObject*>& ObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                               ->GetLayer(PLayerIndex)->GetObjects();

	// 동일 레이어 내에서는 중복을 고려하여 절반의 횟수만 처리하면 된다
	for (size_t i = 0; i < ObjectVector.size(); ++i)
	{
		for (size_t j = i + 1; j < ObjectVector.size(); ++j)
		{
			CollisionCheckWithTypeMatching(ObjectVector[i], ObjectVector[j]);
		}
	}
}

/**
 * @brief 두 레이어 간의 충돌 처리 함수
 *
 * @param PLeftIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index 1
 * @param PRightIndex 레벨 내의 충돌 검사할 Object 정보들이 들어 있는 Layer Index 2
 */
void CCollisionMgr::CollisionBtwLayer(UINT PLeftIndex, UINT PRightIndex)
{
	const vector<CGameObject*>& LeftObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                   ->GetLayer(PLeftIndex)->GetObjects();
	const vector<CGameObject*>& RightObjectVector = CLevelMgr::GetInst()->GetCurrentLevel()
	                                                                    ->GetLayer(PRightIndex)->GetObjects();

	// 서로 다른 레이어의 경우 모든 충돌 검사를 위해서는 sizeL * sizeR 전부 확인해야 한다
	for (size_t i = 0; i < LeftObjectVector.size(); ++i)
	{
		for (size_t j = 0; j < RightObjectVector.size(); ++j)
		{
			CollisionCheckWithTypeMatching(LeftObjectVector[i], RightObjectVector[j]);
		}
	}
}

/**
 * @brief 해당 오브젝트의 충돌 컴포넌트 타입에 맞춰 적절하게 충돌 체크를 진행하는 함수
 *
 * @param PLeftObject Object 1
 * @param PRightObject Object 2
 */
void CCollisionMgr::CollisionCheckWithTypeMatching(const CGameObject* PLeftObject, const CGameObject* PRightObject)
{
	// 1. 2D 충돌체 검사
	if (PLeftObject->Collider2D())
	{
		if (PRightObject->Collider2D())
		{
			CollisionBtwCollider2D(PLeftObject->Collider2D(), PRightObject->Collider2D());
			return;
		}
	}

	// 2. 3D 충돌체 검사
	if (PLeftObject->Collider3D())
	{
		if (PRightObject->Collider3D())
		{
			CollisionBtwCollider3D(PLeftObject->Collider3D(), PRightObject->Collider3D());
			return;
		}

		if (PRightObject->LandScape())
		{
			CollisionBtwLandScape3D(PLeftObject->Collider3D(), PRightObject->LandScape());
			return;
		}

		if (PRightObject->ColliderRay())
		{
			CollisionBtwColliderRay(PRightObject->ColliderRay(), PLeftObject->Collider3D());
			return;
		}
	}

	// 3. LandScape 검사
	if (PLeftObject->LandScape())
	{
		// 3D간 충돌의 경우
		if (PRightObject->Collider3D())
		{
			CollisionBtwLandScape3D(PRightObject->Collider3D(), PLeftObject->LandScape());
			return;
		}

		// Ray와 충돌의 경우
		if (PRightObject->ColliderRay())
		{
			CollisionBtwLandScapeRay(PRightObject->ColliderRay(), PLeftObject->LandScape());
			return;
		}
	}

	// 4. RayCast 검사
	if (PLeftObject->ColliderRay())
	{
		if (PRightObject->Collider3D())
		{
			CollisionBtwColliderRay(PLeftObject->ColliderRay(), PRightObject->Collider3D());
			return;
		}

		// LandScape와 충돌의 경우
		if (PRightObject->LandScape())
		{
			CollisionBtwLandScapeRay(PLeftObject->ColliderRay(), PRightObject->LandScape());
			return;
		}
	}

	// 5. Mesh Collider
	if (PLeftObject->MeshCollider())
	{
		if (PRightObject->MeshCollider())
		{
			CollisionWithMesh(PLeftObject->MeshCollider(), PRightObject->MeshCollider());
			return;
		}

		if (PRightObject->Collider3D())
		{
			CollisionWithMesh(PLeftObject->MeshCollider(), PRightObject->Collider3D());
			return;
		}

		// TODO(KHJ): 나머지 충돌체 판정도 필요하다면 구현할 것
	}
}

// Collider 컴포넌트 충돌체크 후 알맞게 호출
void CCollisionMgr::CollisionBtwCollider2D(CCollider2D* PLeftCol, CCollider2D* PRightCol)
{
	COLLIDER_ID id;
	id.Left = PLeftCol->GetID();
	id.Right = PRightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	// 한번도 등록된 적이 없었다.
	if (iter == m_ColInfo.end())
	{
		// 충돌 조합 등록
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// 한쪽이 Dead상태
	bool IsDead = PLeftCol->GetOwner()->IsDead() || PRightCol->GetOwner()->IsDead();

	// 현재 겹쳐있다.
	if (IsCollision(PLeftCol, PRightCol))
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
				PLeftCol->Overlap(PRightCol);
				PRightCol->Overlap(PLeftCol);
			}
		}
		// 이전에는 떨어져 있었다.
		else
		{
			// 둘중 하나가 dead상태가 아니다
			if (!IsDead)
			{
				PLeftCol->BeginOverlap(PRightCol);
				PRightCol->BeginOverlap(PLeftCol);
			}
			iter->second = true;
		}
	}

	// 현재 떨어져 있다.
	else
	{
		if (iter->second == true)
		{
			PLeftCol->EndOverlap(PRightCol);
			PRightCol->EndOverlap(PLeftCol);
			iter->second = false;
		}
	}
}

void CCollisionMgr::CollisionBtwCollider3D(CCollider3D* PLeftCol, CCollider3D* PRightCol)
{
	COLLIDER_ID id;
	id.Left = PLeftCol->GetID();
	id.Right = PRightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = PLeftCol->GetOwner()->IsDead() || PRightCol->GetOwner()->IsDead();
	bool IsDeactive = PLeftCol->GetState() == DEACTIVE || PRightCol->GetState() == DEACTIVE || PLeftCol->GetOwner()->
		IsDeactivated() || PRightCol->GetOwner()->IsDeactivated();
	bool IsLayerChanged = PLeftCol->GetOwner()->IsLayerMove() || PRightCol->GetOwner()->IsLayerMove();


	// 정책 변경 : 레이어 변경 만으로도 EndOverlap을 시키자.
	// 바뀐 레이어도 충돌된다면 새로 BeginOverlap하는 방향이 맞아 보임

	if (IsCollision3D(PLeftCol, PRightCol))
	{
		if (iter->second)
		{
			// 둘중 하나가 곧 삭제 예정이다.
			if (IsDead || IsDeactive || IsLayerChanged)
			{
				PLeftCol->EndOverlap(PRightCol);
				PRightCol->EndOverlap(PLeftCol);
				iter->second = false;
			}
			else
			{
				PLeftCol->Overlap(PRightCol);
				PRightCol->Overlap(PLeftCol);
			}
		}
		else
		{
			if (!IsDead && !IsDeactive)
			{
				PLeftCol->BeginOverlap(PRightCol);
				PRightCol->BeginOverlap(PLeftCol);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			PLeftCol->EndOverlap(PRightCol);
			PRightCol->EndOverlap(PLeftCol);
			iter->second = false;
		}
	}
}

void CCollisionMgr::CollisionBtwLandScape3D(CCollider3D* PLeftCol, CLandScape* PRightCol)
{
	COLLIDER_ID id;
	id.Left = PLeftCol->GetID();
	id.Right = PRightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = PLeftCol->GetOwner()->IsDead() || PRightCol->GetOwner()->IsDead();
	bool IsDeactive = PLeftCol->GetState() == DEACTIVE || PLeftCol->GetOwner()->IsDeactivated();

	if (IsCollision3DLand(PLeftCol, PRightCol))
	{
		if (iter->second)
		{
			// 둘중 하나가 곧 삭제 예정이다.
			if (IsDead || IsDeactive)
			{
				PLeftCol->EndOverlap(PRightCol);
				iter->second = false;
			}
			else
			{
				PLeftCol->Overlap(PRightCol);
			}
		}
		else
		{
			if (!IsDead && !IsDeactive)
			{
				PLeftCol->BeginOverlap(PRightCol);
				iter->second = true;
			}
		}
	}
	else
	{
		if (iter->second)
		{
			PLeftCol->EndOverlap(PRightCol);
			iter->second = false;
		}
	}
}

void CCollisionMgr::CollisionBtwColliderRay(CColliderRay* PLeftCol, CCollider3D* PRightCol)
{
	// Ray가 트리거를 감지할지 확인하고 판단
	if (!(PLeftCol->IsTriggerTarget()) && PRightCol->IsTrigger())
	{
		//트리거용 타겟을 감지하지 않아야하면 종료
		return;
	}

	COLLIDER_ID id;
	id.Left = PLeftCol->GetID();
	id.Right = PRightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	bool IsDead = PLeftCol->GetOwner()->IsDead() || PRightCol->GetOwner()->IsDead();
	bool IsDeactive = PLeftCol->GetState() == DEACTIVE || PRightCol->GetState() == DEACTIVE || PLeftCol->GetOwner()->
		IsDeactivated() || PRightCol->GetOwner()->IsDeactivated();

	// 단일 타겟이면 Ray용 연산을 위해 연산 후 저장
	if (!(PLeftCol->IsTargetAllMode()))
	{
		if (!(IsDead) || !(IsDeactive))
			IsCollisionRay(PLeftCol, PRightCol);
	}
	else
	{
		if (IsCollisionRay(PLeftCol, PRightCol))
		{
			if (iter->second)
			{
				// 둘중 하나가 곧 삭제 예정이다.
				if (IsDead || IsDeactive)
				{
					PLeftCol->EndOverlap(PRightCol);
					PRightCol->EndOverlap(PLeftCol);
					iter->second = false;
				}
				else
				{
					PLeftCol->Overlap(PRightCol);
					PRightCol->Overlap(PLeftCol);
				}
			}
			else
			{
				if (!IsDead && !IsDeactive)
				{
					PLeftCol->BeginOverlap(PRightCol);
					PRightCol->BeginOverlap(PLeftCol);
					iter->second = true;
				}
			}
		}
		else
		{
			if (iter->second)
			{
				PLeftCol->EndOverlap(PRightCol);
				PRightCol->EndOverlap(PLeftCol);
				iter->second = false;
			}
		}
	}
}

void CCollisionMgr::CollisionBtwLandScapeRay(CColliderRay* PLeftCol, CLandScape* PRightCol)
{
	COLLIDER_ID id;
	id.Left = PLeftCol->GetID();
	id.Right = PRightCol->GetID();

	map<ULONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// LandScape에 Ray정보 등록
	bool IsDead = PLeftCol->GetOwner()->IsDead() || PRightCol->GetOwner()->IsDead();
	bool IsDeactive = PLeftCol->GetState() == DEACTIVE || PLeftCol->GetOwner()->IsDeactivated();

	if (!(IsDead) && !(IsDeactive))
		IsCollisionRayLand(PLeftCol, PRightCol);


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

/** Collision Detail Logic **/

bool CCollisionMgr::IsCollision(CCollider2D* PLeft, CCollider2D* PRight)
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

	Matrix matColLeft = PLeft->GetColliderWorldMat();
	Matrix matColRight = PRight->GetColliderWorldMat();

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
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matColLeft) - XMVector3TransformCoord(
		Vec3(0.f, 0.f, 0.f), matColRight);

	for (int i = 0; i < 4; ++i)
	{
		Vec3 vProj = arrProj[i];
		vProj.Normalize(); // 노말라이즈 해서 투영면의 길이를 1로 만든다.

		// 각도가 90도 이상이면 음수가 나오니 절대값을 해준다.
		float fCenter = fabs(vCenter.Dot(vProj)); // 센터끼리 이은길이
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

bool CCollisionMgr::IsCollision3D(CCollider3D* PLeft, CCollider3D* PRight)
{
	constexpr float EPSILON = 0.0001f;

	// 로컬 큐브의 정점 위치 (8개의 정점)
	static Vec3 arrCube[8] =
	{
		Vec3(-0.5f, 0.5f, 0.5f), // 전면 좌상단
		Vec3(0.5f, 0.5f, 0.5f), // 전면 우상단
		Vec3(0.5f, -0.5f, 0.5f), // 전면 우하단
		Vec3(-0.5f, -0.5f, 0.5f), // 전면 좌하단
		Vec3(-0.5f, 0.5f, -0.5f), // 후면 좌상단
		Vec3(0.5f, 0.5f, -0.5f), // 후면 우상단
		Vec3(0.5f, -0.5f, -0.5f), // 후면 우하단
		Vec3(-0.5f, -0.5f, -0.5f) // 후면 좌하단
	};


	Matrix matColLeft = PLeft->GetColliderWorldMat();
	Matrix matColRight = PRight->GetColliderWorldMat();

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
			if (crossAxis.Length() > EPSILON) // 0벡터 제외
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

	// Right의 꼭짓점 중, Left의 중심에서 가장 가까운 꼭짓점 구하기
	Vec3 closestPoint;
	float minDistSq = FLT_MAX;
	for (int i = 0; i < 8; ++i)
	{
		float distSq = (rightVertices[i] - leftCenter).Length();
		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			closestPoint = rightVertices[i];
		}
	}
	PRight->SetClosestPoint(closestPoint);

	// Left의 꼭짓점 중, Right의 중심에서 가장 가까운 꼭짓점 구하기
	minDistSq = FLT_MAX;
	for (int i = 0; i < 8; ++i)
	{
		float distSq = (leftVertices[i] - rightCenter).Length();
		if (distSq < minDistSq)
		{
			minDistSq = distSq;
			closestPoint = leftVertices[i];
		}
	}
	PLeft->SetClosestPoint(closestPoint);

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
		PLeft->SetHitNormal(-minPenetrationAxis); // Left에서 Right로 향하는 방향
		PRight->SetHitNormal(minPenetrationAxis); // Right에서 Left로 향하는 방향
	}

	return true;
}

bool CCollisionMgr::IsCollisionRay(CColliderRay* PLeftCol, CCollider3D* PRightCol)
{
	// 로컬 큐브의 정점 위치 (8개의 정점)
	static Vec3 arrCube[8] =
	{
		Vec3(-0.5f, 0.5f, 0.5f), // 전면 좌상단
		Vec3(0.5f, 0.5f, 0.5f), // 전면 우상단
		Vec3(0.5f, -0.5f, 0.5f), // 전면 우하단
		Vec3(-0.5f, -0.5f, 0.5f), // 전면 좌하단
		Vec3(-0.5f, 0.5f, -0.5f), // 후면 좌상단
		Vec3(0.5f, 0.5f, -0.5f), // 후면 우상단
		Vec3(0.5f, -0.5f, -0.5f), // 후면 우하단
		Vec3(-0.5f, -0.5f, -0.5f) // 후면 좌하단
	};

	// 충돌체의 월드 행렬
	Matrix matWorld = PRightCol->GetColliderWorldMat();

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
	Vec3 rayPos = PLeftCol->GetRayFinalPos();
	Vec3 rayDir = PLeftCol->GetRayFinalDir();
	float rayMaxDist = PLeftCol->GetRayLength(); // 레이 최대 거리

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

		// 삼각형의 법선 벡터(노말) 계산
		Vec3 edge1 = *triVerts[1] - *triVerts[0];
		Vec3 edge2 = *triVerts[2] - *triVerts[0];
		Vec3 triangleNormal = edge1.Cross(edge2);
		triangleNormal.Normalize();

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
		if (PLeftCol->IsTargetAllMode())
		{
			PLeftCol->SetRayTargetLength(closestDist);
			return true;
		}
		else
		{
			// Update후 정리
			PLeftCol->UpdateRayColInfo(PRightCol->GetOwner(), closestDist);

			m_RayColInfo.insert(PLeftCol);

			return true;
		}
	}


	return false;
}

bool CCollisionMgr::IsCollision3DLand(CCollider3D* PLeftCol, CLandScape* PRightCol)
{
	// 오브젝트는 Transform기준 위치를 발로 잡는다.
	Vec3 ObjectPos = PLeftCol->Transform()->GetWorldPos();

	// 오브젝트 위치 기준으로 LandScape의 높이 측정
	Vec3 LandScapePos = PRightCol->GetWorldPosByLandScape(ObjectPos);

	// 말도 안되는 값이 들어오면 LandScpae위치를 벗어난 위치
	// y축 기준으로 오브젝트가 아래에 있다면 충돌
	if (LandScapePos == Vec3(-10000.f, -10000.f, -10000.f) || ObjectPos.y > LandScapePos.y)
		return false;

	return true;
}

bool CCollisionMgr::IsCollisionRayLand(CColliderRay* PLeftCol, CLandScape* PRightCol)
{
	// Ray의 월드 위치, 방향정보를 받아온다.
	tRayCollision RayInfo;
	RayInfo.RayObj = PLeftCol;
	RayInfo.RayWorldPos = PLeftCol->GetRayFinalPos();
	RayInfo.RayDir = PLeftCol->GetRayFinalDir();
	RayInfo.RayLength = PLeftCol->GetRayLength();

	// LandScape에서 해당 Ray정보를 넘겨준다.(나중에 처리됨)
	PRightCol->AddRayCol(RayInfo);
	m_LandObject.insert(PRightCol);

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

/**
 * @brief 각 메시 충돌체의 바운딩 박스끼리 충돌했는지 여부를 확인하는 함수
 *
 * @param PLeftCollider Mesh Collider 1
 * @param PRightCollider Mesh Collider 2
 * @return 충돌 여부
 */
bool CCollisionMgr::IsBoundingBoxCollided(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider)
{
	// Get Bounding Box
	Vec3 LeftMin, LeftMax, RightMin, RightMax;
	PLeftCollider->GetOwner()->GetWorldBoundingBox(LeftMin, LeftMax);
	PRightCollider->GetOwner()->GetWorldBoundingBox(RightMin, RightMax);

	// AABB Collision Test
	if (LeftMax.x < RightMin.x || LeftMin.x > RightMax.x)
	{
		return false;
	}
	if (LeftMax.y < RightMin.y || LeftMin.y > RightMax.y)
	{
		return false;
	}
	if (LeftMax.z < RightMin.z || LeftMin.z > RightMax.z)
	{
		return false;
	}

	return true;
}

/**
 * @brief 메시 충돌체의 바운딩 박스와 충돌체가 충돌했는지 여부를 확인하는 함수
 *
 * @param PMeshCollider Mesh Collider
 * @param P3DCollider 3D Collider
 * @return 충돌 여부
 */
bool CCollisionMgr::IsBoundingBoxCollided(CMeshCollider* PMeshCollider, const CCollider3D* P3DCollider)
{
	// Get Bounding Box
	Vec3 MeshMin, MeshMax;
	PMeshCollider->GetOwner()->GetWorldBoundingBox(MeshMin, MeshMax);

	// Get 3D Collider Vertexs
	static Vec3 CubeArr[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f),
		Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f),
		Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};

	Matrix AABBMatrix = P3DCollider->GetColliderWorldMat();
	Vec3 AABBVtxs[8];
	for (int i = 0; i < 8; ++i)
	{
		AABBVtxs[i] = XMVector3TransformCoord(CubeArr[i], AABBMatrix);
	}

	// Vertex Check
	for (int i = 0; i < 8; ++i)
	{
		const Vec3& Vtx = AABBVtxs[i];
		if (Vtx.x >= MeshMin.x && Vtx.x <= MeshMax.x &&
			Vtx.y >= MeshMin.y && Vtx.y <= MeshMax.y &&
			Vtx.z >= MeshMin.z && Vtx.z <= MeshMax.z)
			return true;
	}

	return false;
}

/**
 * @brief 실제 메시 충돌체끼리 충돌했는지 여부를 세부적으로 확인하는 함수
 *
 * @param PLeftCollider Mesh Collider 1
 * @param PRightCollider Mesh Collider 2
 * @return 충돌 여부
 */
bool CCollisionMgr::IsMeshCollided(CMeshCollider* PLeftCollider, CMeshCollider* PRightCollider)
{
	// 1. Variable Setting
	Ptr<CMesh> LeftMesh = PLeftCollider->GetMesh();
	Ptr<CMesh> RightMesh = PRightCollider->GetMesh();

	// Early Return
	if (!LeftMesh.Get() || !RightMesh.Get())
	{
		return false;
	}

	UINT LeftVtxCount = LeftMesh->GetVertexCount();
	UINT RightVtxCount = RightMesh->GetVertexCount();

	// Early Return
	if (!LeftVtxCount || !RightVtxCount)
	{
		return false;
	}

	// 2. Triangle Counting
	INT32 LeftTriCount = 0;
	INT32 RightTriCount = 0;

	for (UINT i = 0; i < LeftMesh->GetSubsetCount(); ++i)
	{
		INT32 LeftIdxCount = LeftMesh->GetIndexInfo()[i].IdxCount;
		LeftTriCount += LeftIdxCount / 3;
	}
	for (UINT i = 0; i < RightMesh->GetSubsetCount(); ++i)
	{
		INT32 RightIdxCount = RightMesh->GetIndexInfo()[i].IdxCount;
		RightTriCount += RightIdxCount / 3;
	}

	// Early Return
	if (!LeftTriCount || !RightTriCount)
		return false;

	// 3. Prepare Buffer
	vector<SimpleVtx> LeftVtxVector(LeftVtxCount);
	vector<SimpleVtx> RightVtxVector(RightVtxCount);
	Vtx* LeftVtxArr = static_cast<Vtx*>(LeftMesh->GetVtxSysMem());
	Vtx* RightVtxArr = static_cast<Vtx*>(RightMesh->GetVtxSysMem());
	for (UINT i = 0; i < LeftVtxCount; ++i)
	{
		LeftVtxVector[i].pos = LeftVtxArr[i].vPos;
	}
	for (UINT i = 0; i < RightVtxCount; ++i)
	{
		RightVtxVector[i].pos = RightVtxArr[i].vPos;
	}

	vector<SimpleIdx> LeftIdxVector(LeftTriCount);
	UINT TotalLeftIdx = 0;
	for (UINT i = 0; i < LeftMesh->GetSubsetCount(); ++i)
	{
		UINT* LeftIdxArr = static_cast<UINT*>(LeftMesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < LeftMesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			LeftIdxVector[TotalLeftIdx] = {LeftIdxArr[j * 3], LeftIdxArr[j * 3 + 1], LeftIdxArr[j * 3 + 2]};
			++TotalLeftIdx;
		}
	}

	vector<SimpleIdx> RightIdxVector(RightTriCount);
	UINT TotalRightIdx = 0;
	for (UINT i = 0; i < RightMesh->GetSubsetCount(); ++i)
	{
		UINT* RightIdxArr = static_cast<UINT*>(RightMesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < RightMesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			RightIdxVector[TotalRightIdx] = {RightIdxArr[j * 3], RightIdxArr[j * 3 + 1], RightIdxArr[j * 3 + 2]};
			++TotalRightIdx;
		}
	}

	// 4. Create StructuredBuffer
	CStructuredBuffer LeftVtxBuffer, RightVtxBuffer, LeftIdxBuffer, RightIdxBuffer;
	LeftVtxBuffer.Create(sizeof(SimpleVtx), LeftVtxCount, SB_TYPE::SRV_ONLY, true, LeftVtxVector.data());
	RightVtxBuffer.Create(sizeof(SimpleVtx), RightVtxCount, SB_TYPE::SRV_ONLY, true, RightVtxVector.data());
	LeftIdxBuffer.Create(sizeof(SimpleIdx), LeftTriCount, SB_TYPE::SRV_ONLY, true, LeftIdxVector.data());
	RightIdxBuffer.Create(sizeof(SimpleIdx), RightTriCount, SB_TYPE::SRV_ONLY, true, RightIdxVector.data());

	CStructuredBuffer CountBuffer, OutputBuffer;

	UINT MaxCollision = 4096;
	vector<UINT> CountArr(1);
	vector<CollisionResult> CollisionArr(MaxCollision);
	CountArr[0] = 0;

	CountBuffer.Create(sizeof(UINT), 1, SB_TYPE::SRV_UAV, true, CountArr.data());
	OutputBuffer.Create(sizeof(CollisionResult), MaxCollision, SB_TYPE::SRV_UAV, true, CollisionArr.data());

	// 5. ComputeShader Execute
	m_MeshCollisionCS.SetLeftVertices(&LeftVtxBuffer);
	m_MeshCollisionCS.SetLeftIndices(&LeftIdxBuffer);
	m_MeshCollisionCS.SetRightVertices(&RightVtxBuffer);
	m_MeshCollisionCS.SetRightIndices(&RightIdxBuffer);
	m_MeshCollisionCS.SetCount(&CountBuffer);
	m_MeshCollisionCS.SetResults(&OutputBuffer);
	m_MeshCollisionCS.SetTriCounts(LeftTriCount, RightTriCount);
	m_MeshCollisionCS.Execute();

	// 6. Result Check
	CountBuffer.GetData(CountArr.data());
	OutputBuffer.GetData(CollisionArr.data());

	static int CollisionCount = CountArr[0];

	if (CollisionCount)
	{
		// Calculate Average Normal with Penetration Depth
		Vec3 LeftAverageNormal = {0, 0, 0};
		Vec3 RightAverageNormal = {0, 0, 0};
		float MaxPenetrationDepth;

		for (int i = 0; i < CollisionCount; ++i)
		{
			Vec3 LeftNormal = CollisionArr[i].LeftNormal;
			Vec3 RightNormal = CollisionArr[i].RightNormal;
			float CurrentPenetration = CollisionArr[i].PenetrationDepth;

			LeftAverageNormal += LeftNormal;
			RightAverageNormal += RightNormal;

			// Update Max Depth
			MaxPenetrationDepth = max(MaxPenetrationDepth, min(CurrentPenetration, 1.0f));
		}

		// Calculate Collision Normal
		LeftAverageNormal = LeftAverageNormal / static_cast<float>(CollisionCount);
		RightAverageNormal = RightAverageNormal / static_cast<float>(CollisionCount);
		LeftAverageNormal.Normalize();
		RightAverageNormal.Normalize();

		// Add Safety Margin
		constexpr float SafetyMargin = 0.02f;
		MaxPenetrationDepth = max(0.0f, MaxPenetrationDepth - SafetyMargin);

		// Set Normal & Penetration Depth
		PLeftCollider->SetCollisionNormal(LeftAverageNormal);
		PLeftCollider->SetPenetrationDepth(MaxPenetrationDepth);
		PRightCollider->SetCollisionNormal(RightAverageNormal);
		PRightCollider->SetPenetrationDepth(MaxPenetrationDepth);

		return true;
	}

	return false;
}

/**
 * @brief 메시 충돌체의 각 정점 중 하나라도 3D 충돌체와 충돌했는지 판단하는 함수
 *
 * @param PMeshCollider Mesh Collider
 * @param P3DCollider 3D Collider
 * @return 충돌 여부
 */
bool CCollisionMgr::IsMeshCollided(CMeshCollider* PMeshCollider, CCollider3D* P3DCollider)
{
	// 1. Variable Setting
	Ptr<CMesh> Mesh = PMeshCollider->GetMesh();

	// Early Return
	if (!Mesh.Get())
	{
		return false;
	}

	UINT VtxCount = Mesh->GetVertexCount();

	// Early Return
	if (!VtxCount)
	{
		return false;
	}

	// 2. Triangle Counting
	INT32 TriCount = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		TriCount += Mesh->GetIndexInfo()[i].IdxCount / 3;
	}

	// Early Return
	if (!TriCount)
	{
		return false;
	}

	// 3. Prepare Buffer
	Matrix WorldMatrix = PMeshCollider->GetOwner()->Transform()->GetWorldMat();

	vector<SimpleVtx> VtxVector(VtxCount);
	Vtx* VtxArr = static_cast<Vtx*>(Mesh->GetVtxSysMem());
	for (UINT i = 0; i < VtxCount; ++i)
	{
		VtxVector[i].pos = XMVector3TransformCoord(VtxArr[i].vPos, WorldMatrix);
	}

	vector<SimpleIdx> IdxVector(TriCount);
	UINT TotalIdx = 0;
	for (UINT i = 0; i < Mesh->GetSubsetCount(); ++i)
	{
		UINT* IdxArr = static_cast<UINT*>(Mesh->GetIndexInfo()[i].IdxSysMem);
		for (size_t j = 0; j < Mesh->GetIndexInfo()[i].IdxCount / 3; ++j)
		{
			IdxVector[TotalIdx] = {IdxArr[j * 3], IdxArr[j * 3 + 1], IdxArr[j * 3 + 2]};
			++TotalIdx;
		}
	}

	// Prepare 3D Collider Buffer
	vector<SimpleVtx> AABBVtxVector(8);
	vector<SimpleIdx> AABBIdxVector(12);

	Vec3 CubeArr[8] = {
		Vec3(-0.5f, 0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f),
		Vec3(0.5f, -0.5f, 0.5f), Vec3(-0.5f, -0.5f, 0.5f),
		Vec3(-0.5f, 0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f),
		Vec3(0.5f, -0.5f, -0.5f), Vec3(-0.5f, -0.5f, -0.5f)
	};

	// 3D Collider Vertex
	for (int i = 0; i < 8; ++i)
	{
		AABBVtxVector[i].pos = XMVector3TransformCoord(CubeArr[i], P3DCollider->GetColliderWorldMat());
	}

	// 3D Collider Triangle Index
	uint32_t Triangles[12][3] = {
		{0, 2, 1}, {0, 3, 2}, {1, 5, 6}, {1, 6, 2},
		{4, 5, 6}, {4, 6, 7}, {0, 4, 7}, {0, 7, 3},
		{4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7}
	};

	for (int i = 0; i < 12; ++i)
	{
		AABBIdxVector[i] = {Triangles[i][0], Triangles[i][1], Triangles[i][2]};
	}

	// 4. Create StructuredBuffer
	CStructuredBuffer MeshVtxBuffer, MeshIdxBuffer, AABBVtxBuffer, AABBIdxBuffer;
	MeshVtxBuffer.Create(sizeof(SimpleVtx), VtxCount, SB_TYPE::SRV_ONLY, true, VtxVector.data());
	MeshIdxBuffer.Create(sizeof(SimpleIdx), TriCount, SB_TYPE::SRV_ONLY, true, IdxVector.data());
	AABBVtxBuffer.Create(sizeof(SimpleVtx), 8, SB_TYPE::SRV_ONLY, true, AABBVtxVector.data());
	AABBIdxBuffer.Create(sizeof(SimpleIdx), 12, SB_TYPE::SRV_ONLY, true, AABBIdxVector.data());

	CStructuredBuffer CountBuffer, OutputBuffer;

	UINT MaxCollision = 4096;
	vector<UINT> CountArr(1, 0);
	vector<CollisionResult> CollisionArr(MaxCollision);

	CountBuffer.Create(sizeof(UINT), 1, SB_TYPE::SRV_UAV, true, CountArr.data());
	OutputBuffer.Create(sizeof(CollisionResult), MaxCollision, SB_TYPE::SRV_UAV, true, CollisionArr.data());

	// 5. ComputeShader Execute
	m_MeshCollisionCS.SetLeftVertices(&MeshVtxBuffer);
	m_MeshCollisionCS.SetLeftIndices(&MeshIdxBuffer);
	m_MeshCollisionCS.SetRightVertices(&AABBVtxBuffer);
	m_MeshCollisionCS.SetRightIndices(&AABBIdxBuffer);
	m_MeshCollisionCS.SetCount(&CountBuffer);
	m_MeshCollisionCS.SetResults(&OutputBuffer);
	m_MeshCollisionCS.SetTriCounts(TriCount, 12);
	m_MeshCollisionCS.Execute();

	// 6. Result Check
	CountBuffer.GetData(CountArr.data());
	OutputBuffer.GetData(CollisionArr.data());

	int CollisionCount = CountArr[0];

	if (CollisionCount)
	{
		// Calculate Average Normal with Penetration Depth
		Vec3 LeftAverageNormal = {0, 0, 0};
		Vec3 RightAverageNormal = {0, 0, 0};
		float MaxPenetrationDepth;

		for (int i = 0; i < CollisionCount; ++i)
		{
			Vec3 LeftNormal = CollisionArr[i].LeftNormal;
			Vec3 RightNormal = CollisionArr[i].RightNormal;
			float CurrentPenetration = CollisionArr[i].PenetrationDepth;

			LeftAverageNormal += LeftNormal;
			RightAverageNormal += RightNormal;

			// Update Max Depth
			MaxPenetrationDepth = max(MaxPenetrationDepth, min(CurrentPenetration, 1.0f));
		}

		// Calculate Collision Normal
		LeftAverageNormal = LeftAverageNormal / static_cast<float>(CollisionCount);
		RightAverageNormal = RightAverageNormal / static_cast<float>(CollisionCount);
		LeftAverageNormal.Normalize();
		RightAverageNormal.Normalize();

		// Add Safety Margin
		constexpr float SafetyMargin = 0.02f;
		MaxPenetrationDepth = max(0.0f, MaxPenetrationDepth - SafetyMargin);

		// Set Normal & Penetration Depth
		PMeshCollider->SetCollisionNormal(LeftAverageNormal);
		PMeshCollider->SetPenetrationDepth(MaxPenetrationDepth);
		P3DCollider->SetHitNormal(RightAverageNormal);
		P3DCollider->SetPenetrationDepth(MaxPenetrationDepth);

		return true;
	}

	return false;
}

/** Collision In LandScape **/

void CCollisionMgr::LandCheak()
{
	// LandScape에서 ray연산호출 이후 값 보관
	set<CLandScape*>::iterator Lnaditer = m_LandObject.begin();
	for (; Lnaditer != m_LandObject.end(); ++Lnaditer)
	{
		CLandScape* pLandscape = *Lnaditer;

		// 연산 이후 데이터 받아오기
		vector<tRayCollision>& RayCol = pLandscape->Collidercalcul();

		// 연산된 데이터 처리
		for (size_t i = 0; i < RayCol.size(); ++i)
		{
			// 레이 콜라이더 찾기
			CColliderRay* Ray = static_cast<CColliderRay*>(RayCol[i].RayObj);

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
				bool IsDeactive = Ray->GetState() == DEACTIVE || Ray->GetOwner()->IsDeactivated();

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
				Ray->SetRayTargetLength(static_cast<float>(RayCol[i].Distance));
			}
			// 단일 타겟 모드 시
			else
			{
				m_RayColInfo.insert(Ray);

				if (RayCol[i].Success == 1)
				{
					// 단일타겟모드시 ray콜라이더에 전용 데이터 삽입
					Ray->UpdateRayColInfo(pLandscape->GetOwner(), static_cast<float>(RayCol[i].Distance));
				}
			}
		}
	}
}

/** Additional Calc **/

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
			bool IsDeactive = pRay->GetState() == DEACTIVE || pRay->GetOwner()->IsDeactivated();

			// LandScape인지 3D인지 구분해서 처리
			if (RightObject->Collider3D()) // 3D타입
			{
				CCollider3D* p3DCol = RightObject->Collider3D();

				IsDeactive = p3DCol->GetState() == DEACTIVE || p3DCol->GetOwner()->IsDeactivated();

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
			else if (RightObject->LandScape()) // LANDSCAPE 타입
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

		// LandScape인지 3D인지 구분해서 처리
		if (RightObject->Collider3D()) // 3D타입
		{
			CCollider3D* p3DCol = RightObject->Collider3D();

			// 서로 떨어진 것은 확정
			if (iter->second)
			{
				pRay->EndOverlap(p3DCol);
				p3DCol->EndOverlap(pRay);
				iter->second = false;
			}
		}
		else if (RightObject->LandScape()) // LANDSCAPE 타입
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

#pragma once

union COLLIDER_ID
{
	struct
	{
		UINT Left;
		UINT Right;
	};

	ULONGLONG ID;
};

class CCollider2D;
class CCollider3D;
class CColliderRay;
class CLandScape;

class CCollisionMgr :
	public singleton<CCollisionMgr>
{
	SINGLE(CCollisionMgr);
private:
	UINT                            m_Matrix[MAX_LAYER];
	map<ULONGLONG, bool>            m_ColInfo;
	set<CColliderRay*>				m_RayColInfo;		// 레이 충돌처리를 위해 저장된 데이터
	set<CLandScape*>				m_LandObject;		// LandScape콜라이더

public:
	void CollisionCheck(UINT _Left, UINT _Right);
	void CollisionCheckClear() { memset(m_Matrix, 0, sizeof(UINT) * MAX_LAYER); }

private:
	void CollisionBtwLayer(UINT _Left, UINT _Right);
	void CollisionBtwCollider2D(CCollider2D* _LeftCol, CCollider2D* _RightCol);
	void CollisionBtwCollider3D(CCollider3D* _LeftCol, CCollider3D* _RightCol);
	void CollisionBtwLandScape3D(CCollider3D* _LeftCol, CLandScape* _RightCol);
	void CollisionBtwColliderRay(CColliderRay* _LeftCol, CCollider3D* _RightCol);
	void CollisionBtwLandScapeRay(CColliderRay* _LeftCol, CLandScape* _RightCol);

	void LandCheak();
	void RayOverlapCheak();

	bool IsCollision(CCollider2D* _Left, CCollider2D* _Right);
	bool IsCollision3D(CCollider3D* _Left, CCollider3D* _Right);
	bool IsCollisionRay(CColliderRay* _LeftCol, CCollider3D* _RightCol);
	bool IsCollision3DLand(CCollider3D* _LeftCol, CLandScape* _RightCol);
	bool IsCollisionRayLand(CColliderRay* _LeftCol, CLandScape* _RightCol);

public:


public:

	void Tick();

};

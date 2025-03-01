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

class CCollisionMgr :
    public singleton<CCollisionMgr>
{
    SINGLE(CCollisionMgr)
    UINT m_Matrix[MAX_LAYER];
    map<ULONGLONG, bool> m_ColInfo;

public:
    void CollisionCheck(UINT _Left, UINT _Right);
    void CollisionCheckClear() { memset(m_Matrix, 0, sizeof(UINT) * MAX_LAYER); }

private:
    void CollisionBtwLayer(UINT _Left, UINT _Right);
    void CollisionBtwCollider2D(CCollider2D* _LeftCol, CCollider2D* _RightCol);
	void CollisionBtwCollider3D(CCollider3D* _LeftCol, CCollider3D* _RightCol);
    bool IsCollision(CCollider2D* _Left, CCollider2D* _Right);
	bool IsCollision3D(CCollider3D* _Left, CCollider3D* _Right);

public:
    void Tick();
};

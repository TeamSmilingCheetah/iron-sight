#include "pch.h"
#include "CCollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CCollider2D.h"

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

    // 충돌 검사를 하려는 두 레이어가 다른 레이어인경우
    if (_Left != _Right)
    {
        for (size_t i = 0; i < vecLeft.size(); ++i)
        {
            if (nullptr == vecLeft[i]->Collider2D())
                continue;

            for (size_t j = 0; j < vecRight.size(); ++j)
            {
                if (nullptr == vecRight[j]->Collider2D())
                    continue;

                CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
            }
        }
    }

    // 두 레이어가 동일한 경우
    else
    {
        for (size_t i = 0; i < vecLeft.size(); ++i)
        {
            if (nullptr == vecLeft[i]->Collider2D())
                continue;

            for (size_t j = i + 1; j < vecRight.size(); ++j)
            {
                if (nullptr == vecRight[j]->Collider2D())
                    continue;

                CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
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

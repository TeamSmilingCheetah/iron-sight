#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CCollider3D;

class CColliderRay :
    public CComponent
{
private:
    Vec3        m_Offset;
    tRay        m_RayPosDir;
    Matrix      m_matColliderWorld; // 크기, 회전, 이동

    Vec3        m_RayFinalPos;    // 최종 레이의 위치
    Vec3        m_RayFinalDir;      // 최종 레이의 방향

    float        m_RayLength;
    float       m_RayTargetLength;

    bool        m_RayTargetOne;         // 레이가 발견 가능한 타겟은 하나(가장 가까운 오브젝트 연산)

    int         m_OverlapCount;

public:
    void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
    void SetRayPos(Vec3 _Pos) { m_RayPosDir.vStart = _Pos; }
    void SetRayDir(Vec3 _Dir) { m_RayPosDir.vDir = _Dir; m_RayPosDir.vDir.Normalize(); }
	void SetRayLength(float _Length) { m_RayLength = _Length; }

    tRay GetRay() { return m_RayPosDir; }
    Vec3 GetOffset() { return m_Offset; }
    float GetRayLength() { return m_RayLength; }

    Vec3 GetRayFinalPos() { return m_RayFinalPos; }
    Vec3 GetRayFinalDir() { return m_RayFinalDir; }

    const Matrix& GetColliderWorldMat() { return m_matColliderWorld; }

public:
    virtual void FinalTick() override;

    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void BeginOverlap(CCollider3D* _Other);
    void Overlap(CCollider3D* _Other);
    void EndOverlap(CCollider3D* _Other);

public:
    CLONE(CColliderRay);
    CColliderRay();
    CColliderRay(const CColliderRay& _Origin);
    ~CColliderRay();




};


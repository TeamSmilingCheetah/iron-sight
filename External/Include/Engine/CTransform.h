#pragma once

#include "CComponent.h"

class CTransform :
    public CComponent
{
private:
    Vec3        m_RelativePos;
    Vec3        m_RelativeScale;
    Vec3        m_RelativeRotation;

    Vec3        m_LocalDir[3];
    Vec3        m_WorldDir[3];

    Matrix      m_matWorld;
    Matrix      m_matWorldInv;

    float       m_FrustumRadius;    // Frustum 체크할 때 사용할 반경정보

    bool        m_IndependentScale;
    bool        m_FrustumCheck;


public:
    void SetRelativePos(Vec3 _Pos) { m_RelativePos = _Pos; }
    void SetRelativePos(float _x, float _y, float _z) { m_RelativePos = Vec3(_x, _y, _z); }

    void SetRelativeScale(Vec3 _Scale) { m_RelativeScale = _Scale; }
    void SetRelativeScale(float _x, float _y, float _z) { m_RelativeScale = Vec3(_x, _y, _z); }

    void SetRelativeRotation(Vec3 _Rotation) { m_RelativeRotation = _Rotation; }
    void SetRelativeRotation(float _x, float _y, float _z) { m_RelativeRotation = Vec3(_x, _y, _z); }

    void SetIndependentScale(bool _Scale) { m_IndependentScale = _Scale; }

    Vec3 GetRelativePos() { return m_RelativePos; }
    Vec3 GetRelativeScale() { return m_RelativeScale; }
    Vec3 GetRelativeRotation() { return m_RelativeRotation; }

    Vec3 GetWorldPos() { return m_matWorld.Translation(); }
    Vec3 GetWorldScale();

    const Matrix& GetWorldMat() const { return m_matWorld; }
    const Matrix& GetWorldInvMat() const { return m_matWorldInv; }
    void SetWorldMat(const Matrix& _matWorld) { m_matWorld = _matWorld; }

    Vec3 GetLocalDir(DIR_TYPE _Type) { return m_LocalDir[(UINT)_Type]; }
    Vec3 GetWorldDir(DIR_TYPE _Type) { return m_WorldDir[(UINT)_Type]; }

    void Binding();

    void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
    bool IsFrustumCheck() { return m_FrustumCheck; }

    float GetFrustumRadius() { return m_FrustumRadius; }
    void SetFrustumRadius(float _Radius) { m_FrustumRadius = _Radius; }

public:
    virtual void FinalTick() override;

    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _FILE) override;

public:
    CLONE(CTransform);
    CTransform();
    ~CTransform();
};


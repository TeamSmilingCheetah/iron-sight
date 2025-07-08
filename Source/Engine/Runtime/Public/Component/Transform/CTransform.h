#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CTransform :
	public CComponent
{
	Vec3 m_RelativePos;
	Vec3 m_RelativeScale;
	Vec3 m_RelativeRotation;

	Quaternion m_Quaternion;

	Vec3 m_LocalDir[3];
	Vec3 m_WorldDir[3];

	Matrix m_matWorld;
	Matrix m_matWorldInv;

	bool m_IndependentScale;
	bool m_FrustumCheckRequired;

	bool m_ManualUpdate;	// true: transform을 외부에서 세팅받음. final tick을 skip 함.

public:
	void SetRelativePos(Vec3 _Pos) { m_RelativePos = _Pos; }
	void SetRelativePos(float _x, float _y, float _z) { m_RelativePos = Vec3(_x, _y, _z); }

	void SetRelativeScale(Vec3 _Scale) { m_RelativeScale = _Scale; }
	void SetRelativeScale(float _x, float _y, float _z) { m_RelativeScale = Vec3(_x, _y, _z); }

	void SetRelativeScaleMultiply(float PScale) { m_RelativeScale *= PScale; }

	void SetRelativeRotation(Vec3 _Rotation);
	void SetRelativeRotation(float _x, float _y, float _z);
	void RotateAxis(Vec3 _Axis, float _Angle);

	void SetIndependentScale(bool _Scale) { m_IndependentScale = _Scale; }

	Vec3 GetRelativePos() const { return m_RelativePos; }
	Vec3 GetRelativeScale() const { return m_RelativeScale; }
	Vec3 GetRelativeRotation() const { return m_RelativeRotation; }

	Vec3 GetWorldPos() const { return m_matWorld.Translation(); }
	Vec3 GetWorldScale();

	const Matrix& GetWorldMat() const { return m_matWorld; }
	const Matrix& GetWorldInvMat() const { return m_matWorldInv; }
	void SetWorldMat(const Matrix& _matWorld) { m_matWorld = _matWorld; }

	Vec3 GetLocalDir(DIR_TYPE _Type) const { return m_LocalDir[static_cast<UINT>(_Type)]; }
	Vec3 GetWorldDir(DIR_TYPE _Type) const { return m_WorldDir[static_cast<UINT>(_Type)]; }

	void Binding();

	void SetFrustumCheck(bool _Check) { m_FrustumCheckRequired = _Check; }
	bool FrustumCheckRequired() const { return m_FrustumCheckRequired; }

	bool IsIndependentScale() const { return m_IndependentScale; }

	bool IsManualUpdate() const { return m_ManualUpdate; }
	void SetManualUpdate(bool _b) { m_ManualUpdate = _b; }

	void FinalTick() override;

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _FILE) override;

	CLONE(CTransform);
	CTransform();
	~CTransform() override;
};

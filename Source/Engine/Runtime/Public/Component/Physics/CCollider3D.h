#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CCollider3D :
	public CComponent
{
private:
	Vec3        m_Offset;
	Vec3        m_Scale;
	Vec3        m_FinalPos;
	Matrix      m_matColliderWorld; // 크기, 회전, 이동
	bool        m_IndependentScale;

	int         m_OverlapCount;

	COLLIDER_STATE  m_State;

public:
	void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
	void SetScale(Vec3 _Scale) { m_Scale = _Scale; }

	Vec3 GetOffset() { return m_Offset; }
	Vec3 GetScale() { return m_Scale; }

	const Matrix& GetColliderWorldMat() { return m_matColliderWorld; }

	void SetIndependentScale(bool _Scale) { m_IndependentScale = _Scale; }
	bool IsIndependentScale() { return m_IndependentScale; }

	COLLIDER_STATE GetState() { return m_State; }
	bool IsActive() { return m_State == ACTIVE; }

public:
	virtual void FinalTick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	void BeginOverlap(CCollider3D* _Other);
	void Overlap(CCollider3D* _Other);
	void EndOverlap(CCollider3D* _Other);

	void BeginOverlap(CColliderRay* _Other);
	void Overlap(CColliderRay* _Other);
	void EndOverlap(CColliderRay* _Other);

	void BeginOverlap(CLandScape* _Other);
	void Overlap(CLandScape* _Other);
	void EndOverlap(CLandScape* _Other);

public:
	CLONE(CCollider3D);
	CCollider3D();
	CCollider3D(const CCollider3D& _Origin);
	~CCollider3D();

};


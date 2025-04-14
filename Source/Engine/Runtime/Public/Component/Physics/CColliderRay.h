#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CCollider3D;
class CLandScape;

struct RAYCOLLIDERDATA
{
	CColliderRay* RayObject;
	CGameObject* HitObject;
	CGameObject* PrevObject;
	float           Length;

	bool operator<(const RAYCOLLIDERDATA& other) const
	{
		return Length < other.Length;
	}
	RAYCOLLIDERDATA()
		: RayObject(nullptr)
		, HitObject(nullptr)
		, PrevObject(nullptr)
		, Length(1000000.f)
	{

	}

	RAYCOLLIDERDATA(CColliderRay* _Ray, CGameObject* _Right, float _Length)
		: RayObject(_Ray)
		, HitObject(_Right)
		, PrevObject(nullptr)
		, Length(_Length)
	{

	}
};

class CColliderRay :
	public CComponent
{
private:
	Vec3        m_Offset;
	tRay        m_RayPosDir;
	Matrix      m_matColliderWorld; // 크기, 회전, 이동

	Vec3        m_RayFinalPos;    // 최종 레이의 위치
	Vec3        m_RayFinalDir;      // 최종 레이의 방향

	float        m_RayLength;			// Ray길이
	float       m_RayTargetLength;		// 디버그용 임시 길이

	int         m_OverlapCount;

	COLLIDER_STATE  m_State;					// 충돌체 상태
	RAYCOLLIDERDATA         m_RayColInfo;		// 단일 타겟 용 검사 구조체

	bool		m_IndependentDir;		// 독립적인 방향
	bool        m_RayTargetAll;         // 레이가 발견 가능한 타겟 판정


public:
	void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
	void SetRayPos(Vec3 _Pos) { m_RayPosDir.vStart = _Pos; }
	void SetRayDir(Vec3 _Dir) { m_RayPosDir.vDir = _Dir; m_RayPosDir.vDir.Normalize(); }
	void SetRayLength(float _Length) { m_RayLength = _Length; }
	void RayTargetMode(bool _bool) { m_RayTargetAll = _bool; }
	void SetRayTargetLength(float _TargetLength) { m_RayTargetLength = _TargetLength; }
	void SetIndependentDir(bool _true) { m_IndependentDir = _true; }

	tRay GetRay() { return m_RayPosDir; }
	Vec3 GetRayPos() { return m_RayPosDir.vStart; }
	Vec3 GetRayDir() { return m_RayPosDir.vDir; }
	Vec3 GetOffset() { return m_Offset; }
	float GetRayLength() { return m_RayLength; }

	Vec3 GetRayFinalPos() { return m_RayFinalPos; }
	Vec3 GetRayFinalDir() { return m_RayFinalDir; }
	bool IsIndependentDir() { return m_IndependentDir; }

	const Matrix& GetColliderWorldMat() { return m_matColliderWorld; }

	RAYCOLLIDERDATA& GetTargetInfoRef() { return m_RayColInfo; }

	bool IsTargetAllMode() { return m_RayTargetAll; }

	COLLIDER_STATE GetState() { return m_State; }
	bool IsActive() { return m_State == ACTIVE; }

	bool UpdateRayColInfo(CGameObject* _HitObject, float _Distance);
	void ClearRayColInfo();

	// 충돌체 화성화
	void Activate();

	// 충돌체 비활성화
	void Deactivate();

public:
	virtual void FinalTick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	void BeginOverlap(CCollider3D* _Other);
	void Overlap(CCollider3D* _Other);
	void EndOverlap(CCollider3D* _Other);

	void BeginOverlap(CLandScape* _Other);
	void Overlap(CLandScape* _Other);
	void EndOverlap(CLandScape* _Other);

public:
	CLONE(CColliderRay);
	CColliderRay();
	CColliderRay(const CColliderRay& _Origin);
	~CColliderRay();


	friend class CColliderMgr;

};


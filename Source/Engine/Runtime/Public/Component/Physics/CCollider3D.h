#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

/**
 * @brief 오브젝트에 충돌체 속성을 추가하는 컴포넌트 클래스
 *
 * @param m_Offset 충돌체의 로컬 위치 오프셋
 * @param m_Scale 충돌체의 크기 스케일
 * @param m_RotY 충돌체의 Y축 회전값
 * @param m_FinalPos 충돌체의 최종 월드 위치
 * @param m_HitNormal 충돌 시 충돌면의 법선 벡터
 * @param m_HitPoint 충돌 시 충돌 지점의 위치
 * @param m_matColliderWorld 충돌체의 월드 변환 행렬 (크기, 회전, 이동 정보 포함)
 * @param m_OverlapCount 현재 겹침 상태인 다른 충돌체의 수
 * @param m_Status 충돌체의 상태 플래그 (트리거, 독립 회전 등)
 * @param m_State 충돌체의 활성화 상태
 * @param m_IndependentScale 오브젝트의 스케일과 독립적으로 동작하는지 여부
 */
class CCollider3D :
	public CComponent
{
private:
	Vec3        m_Offset;
	Vec3        m_Scale;
	float		m_RotY;
	Vec3        m_FinalPos;
	Vec3		m_HitNormal;
	Vec3		m_HitPoint;
	Vec3		m_ClosestPoint;
	Matrix      m_matColliderWorld; // 크기, 회전, 이동

	int         m_OverlapCount;
	int			m_Status;
	float		m_PenetrationDepth;

	COLLIDER_STATE  m_State;

	bool        m_IndependentScale;

public:
	void FinalTick() override;
	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;

	void SetTrigger(bool _true);
	void SetIndependentRot(bool _true);

	void BeginOverlap(CCollider3D* POther);
	void Overlap(CCollider3D* POther);
	void EndOverlap(CCollider3D* POther);

	void BeginOverlap(CColliderRay* POther);
	void Overlap(CColliderRay* POther);
	void EndOverlap(CColliderRay* POther);

	void BeginOverlap(CLandScape* POther);
	void Overlap(CLandScape* POther);
	void EndOverlap(CLandScape* POther);

	void BeginOverlap(CMeshCollider* POther);
	void Overlap(CMeshCollider* POther);
	void EndOverlap(CMeshCollider* POther);

	void Activate();
	void Deactivate();

	// Special Member Function
	CLONE(CCollider3D);
	CCollider3D();
	CCollider3D(const CCollider3D& POrigin);
	~CCollider3D() override;

	// Getter & Setter
	void SetOffset(Vec3 POffset) { m_Offset = POffset; }
	void SetScale(Vec3 PScale) { m_Scale = PScale; }
	void SetScaleMultiplier(float PSize) { m_Scale *= PSize; }
	void SetRotY(float PRot) { m_RotY = PRot; }
	void SetHitNormal(Vec3 PNor) { m_HitNormal = PNor; }
	void SetHitPoint(Vec3 PhitPoint) { m_HitPoint = PhitPoint; }
	void SetIndependentScale(bool PScale) { m_IndependentScale = PScale; }
	void SetClosestPoint(Vec3 PclosestPoint) { m_ClosestPoint = PclosestPoint; }
	void SetPenetrationDepth(float PPenetrationDepth) { m_PenetrationDepth = PPenetrationDepth; }

	Vec3 GetOffset() const { return m_Offset; }
	Vec3 GetScale() const { return m_Scale; }
	float GetRotY() const { return m_RotY; }
	Vec3 GetHitNormal() const { return m_HitNormal; }
	Vec3 GetHitPoint() const { return m_HitPoint; }
	Vec3 GetClosestPoint() { return m_ClosestPoint; }
	float GetPenetrationDepth() const { return m_PenetrationDepth; }
	COLLIDER_STATE GetState() const { return m_State; }

	bool IsTrigger() const { return (m_Status & TRIGGER); }
	bool IsIndependentRot() const { return (m_Status & INDEPENDENT_ROT); }

	const Matrix& GetColliderWorldMat() const { return m_matColliderWorld; }

	bool IsIndependentScale() const { return m_IndependentScale; }
	bool IsActive() const { return m_State == ACTIVE; }
};

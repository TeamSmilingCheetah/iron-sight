#pragma once
#include "ColliderBase.h"

class FCollider2D :
    public IColliderBase
{
private:
    Vec2 Offset;
    Vec2 Scale;
    Vec2 FinalPos;
    Matrix MatrixColliderWorld;
    bool bIndependentScale;

public:
    void FinalTick() override;
    const AABB GetAABB() const override;
    void SaveComponent(FILE* InFile) override;
    void LoadComponent(FILE* InFile) override;

	// Getter & Setter
	Vec2 GetOffset() const { return Offset; }
	Vec2 GetScale() const { return Scale; }
	const Matrix& GetColliderWorldMatrix() const { return MatrixColliderWorld; }
	bool IsIndependentScale() const { return bIndependentScale; }

	void SetOffset(Vec2 InOffset) { Offset = InOffset; }
	void SetScale(Vec2 InScale) { Scale = InScale; }
    void SetIndependentScale(bool InScale) { bIndependentScale = InScale; }

	// Special Member Function
    FCollider2D();
    FCollider2D(const FCollider2D& POrigin);
    ~FCollider2D() override;
    CLONE(FCollider2D);
};

#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/Runtime/Public/Component/Script/CScript.h"

/**
 * @brief mesh에 충돌체 성질을 부여하는 컴포넌트
 *
 * @param MMeshPtr 충돌의 기준이 될 mesh
 * @param MOverlapCount 현재 Overlap 중첩 횟수를 기록
 * @param MCollisionNormal mesh 충돌 시 충돌한 면의 법선 벡터
 */
class CMeshCollider :
	public CComponent
{
	friend class CColliderMgr;

private:
	Ptr<CMesh> MMeshPtr;
	int MOverlapCount;
	Vec3 MCollisionNormal;

public:
	void FinalTick() override;
	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;

	void Init() override;

	// Getter & Setter
	Ptr<CMesh> GetMesh() const { return MMeshPtr; }
	void SetMesh(CMesh* PMesh) { MMeshPtr = PMesh; }
	Vec3& GetCollisionNormal() { return MCollisionNormal; }
	void SetCollisionNormal(Vec3 PCollisionNormal) { MCollisionNormal = PCollisionNormal; }

	// Templated Overlap Function
	template<typename T>
	void BeginOverlap(T* POther);

	template<typename T>
	void Overlap(T* POther);

	template<typename T>
	void EndOverlap(T* POther);

	// Special Member Function
	CLONE(CMeshCollider);
	CMeshCollider();
	CMeshCollider(const CMeshCollider& POrigin);
	~CMeshCollider() override;
};

/** Overlap Fuction **/

template<typename T>
void CMeshCollider::BeginOverlap(T* POther)
{
	++MOverlapCount;

	for (auto EachScript : GetOwner()->GetScripts())
	{
		EachScript->BeginOverlap(this, POther->GetOwner(), POther);
	}
}

template<typename T>
void CMeshCollider::Overlap(T* POther)
{
	for (auto EachScript : GetOwner()->GetScripts())
	{
		EachScript->Overlap(this, POther->GetOwner(), POther);
	}
}

template<typename T>
void CMeshCollider::EndOverlap(T* POther)
{
	--MOverlapCount;

	for (auto EachScript : GetOwner()->GetScripts())
	{
		EachScript->EndOverlap(this, POther->GetOwner(), POther);
	}
}

#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CStructuredBuffer;

class CSkeleton :
	public CAsset
{
private:
	vector<tMTBone> m_vecBones;

	vector<Matrix> m_vecOffset;
	CStructuredBuffer* m_BoneInvBuffer; // 각 뼈의 상쇄(무효화) 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)

	vector<int> m_vecParent;
	CStructuredBuffer* m_BoneParentBuffer; // 각 뼈의 부모 뼈 인덱스를 저장한 structured buffer -> Debug용

public:
	const vector<tMTBone>* GetBones() const { return &m_vecBones; }
	UINT GetBoneCount() const { return static_cast<UINT>(m_vecBones.size()); }

	CStructuredBuffer* GetBoneInverseBuffer() const { return m_BoneInvBuffer; } // 각 Bone 의 Inverse 행렬
	CStructuredBuffer* GetBoneParentBuffer() const { return m_BoneParentBuffer; }

public:
	static Ptr<CSkeleton> LoadFromFBX(CFBXLoader& _loader);

	virtual int Save(const wstring& _RelativePath) override;
	virtual int Load(const wstring& _RelativePath) override;

public:
	CLONE_DISABLE(CSkeleton);
	CSkeleton(bool _bEngineRes = false);
	~CSkeleton();
};

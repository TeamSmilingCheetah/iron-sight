#pragma once
#include "CAsset.h"

class CStructuredBuffer;

class CSkeleton :
	public CAsset
{
private:
	vector<tMTBone>		m_vecBones;

	vector<Matrix> vecOffset;
	CStructuredBuffer*  m_BoneInvBuffer; // 각 뼈의 상쇄(무효화) 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)

public:
	const vector<tMTBone>* GetBones() const { return &m_vecBones; }
	UINT GetBoneCount() const { return static_cast<UINT>(m_vecBones.size()); }

	CStructuredBuffer* GetBoneInverseBuffer() const { return m_BoneInvBuffer; } // 각 Bone 의 Inverse 행렬

public:
	static Ptr<CSkeleton> LoadFromFBX(CFBXLoader& _loader);

	virtual int Save(const wstring& _RelativePath) override;
	virtual int Load(const wstring& _RelativePath) override;

public:
	CLONE_DISABLE(CSkeleton);
	CSkeleton(bool _bEngineRes = false);
	~CSkeleton();
};


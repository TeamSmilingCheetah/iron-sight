#pragma once
#include "CSkeleton.h"
#include "Engine/System/Public/Asset/Base/CAsset.h"

class CStructuredBuffer;

// 3d model animation
class CAnimation :
	public CAsset
{
private:
	// Bone 정보
	Ptr<CSkeleton> m_Skeleton;

	// Animation3D 정보
	int m_StartFrame;
	int m_EndFrame;
	int m_FrameLength;

	double m_StartTime;
	double m_EndTime;
	double m_TimeLength;

	FbxTime::EMode m_TimeMode;

	// KeyFrame 정보
	vector<tFrameTrans>		m_vecKeyFrames;
	CStructuredBuffer*		m_BoneFrameData; // 전체 본 프레임 정보(크기, 이동, 회전) (프레임 개수만큼)

public:
	CStructuredBuffer* GetBoneFrameDataBuffer() const { return m_BoneFrameData; } // 전체 본 프레임 정보
	CStructuredBuffer* GetBoneInverseBuffer() const { return m_Skeleton->GetBoneInverseBuffer(); }
	CStructuredBuffer* GetBoneParentBuffer() const { return m_Skeleton->GetBoneParentBuffer(); }

	const vector<tMTBone>* GetBones() const { return m_Skeleton->GetBones(); }
	UINT GetBoneCount() const { return m_Skeleton->GetBoneCount(); }

	double GetStartTime() const { return m_StartTime; }
	double GetEndTime() const { return m_EndTime; }
	double GetTimeLength() const { return m_TimeLength; }

	int GetFrameLength() const { return m_FrameLength; }

	void SetTimeMode(FbxTime::EMode _Mode) { m_TimeMode = _Mode; }

public:
	static vector<Ptr<CAnimation>> LoadFromFBX(CFBXLoader& _loader);

	virtual int Save(const wstring& _RelativePath) override;
	virtual int Load(const wstring& _RelativePath) override;

public:
	CLONE_DISABLE(CAnimation);
	CAnimation(bool _bEngineRes = false);
	~CAnimation();
};

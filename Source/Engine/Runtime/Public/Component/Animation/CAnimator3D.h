#pragma once
#include "Engine/System/Public/Asset/Mesh/CMesh.h"
#include "Engine/Runtime/Public/Component/Base/CComponent.h"
#include "Engine/System/Public/Asset/Animation/CAnimation.h"

class CAnimation;
class CStructuredBuffer;
class CMeshRender;
class CGameObject;

class CAnimator3D :
	public CComponent
{
private:
	vector<Ptr<CAnimation>>		m_vecClip;

	float			m_AccTime;
	int				m_FPS; // 30
	float			m_FrameDuration;

	vector<Matrix>	m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
	int				m_CurClip; // 클립 인덱스

	int				m_FrameIdx; // 클립의 현재 프레임
	int				m_NextFrameIdx; // 클립의 다음 프레임
	float			m_Ratio; // 프레임 사이 비율

	CStructuredBuffer* m_BoneFinalMatBuffer; // 특정 프레임(보간)의 최종 행렬
	CStructuredBuffer* m_BonePureMatBuffer;  // 특정 프레임(보간)의 bone 행렬 (bone inverse가 곱해지지 않은)
	bool m_bFinalMatUpdate; // 최종행렬 연산 수행여부

	CMeshRender*	m_BindCaller;	// Bind를 호출한 MeshRenderer 기억

	// Bone에 대응되는 오브젝트를 저장
	vector<CGameObject*>	m_vecBoneObject;

	unordered_map<wstring, CGameObject*> m_mapBoneObject;

	vector<Matrix>			m_vecBoneWorldTransform;

	bool	m_Active;

public:
	void AddAnimClip(Ptr<CAnimation> _pAnim);
	void SetAnimClip(const vector<Ptr<CAnimation>>& _vecAnim);

	void ClearAnimClip() { m_vecClip.clear(); }

	int GetCurClipIdx() const { return m_CurClip; }
	double GetCurClipTime() const { return m_FrameDuration * m_CurClip + m_AccTime; }
	int GetCurFrameIdx() const { return m_FrameIdx; }
	int GetFrameLength() const { return m_vecClip[m_CurClip]->GetFrameLength(); }
	float GetRatio() const { return m_Ratio; }
	bool IsActive() const { return m_Active; }

	const vector<Ptr<CAnimation>>& GetClips() const { return m_vecClip; }
	void SetCurClip(int _Idx);
	void SetCurFrame(int _FrameIdx);

	void Play() { m_Active = true; }
	void Pause() { m_Active = false; }

	UINT GetBoneCount() const { return m_vecClip[m_CurClip]->GetBoneCount(); }

	vector<CGameObject*> GetvecBone() { return m_vecBoneObject; }
	unordered_map<wstring, CGameObject*> GetmapBone() { return m_mapBoneObject; }
	vector<Matrix> GetvecBoneWorldTrasnform() { return m_vecBoneWorldTransform; }

	CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
	void ClearData();

	// TEST: 일단 여기에 Crop 기능 넣음. 웬만하면 Editor에서만 허용되도록 하고 싶음
	void Crop(int _StartIdx, int _EndIdx);


private:
	void CreateBoneObject();
	void LinkBoneObject();

public:
	void Binding(CMeshRender* _Renderer);
	void Begin() override;
	void FinalTick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	virtual void SetOwner(CGameObject* _Owner) override;

public:
	CLONE(CAnimator3D);
	CAnimator3D();
	CAnimator3D(const CAnimator3D& _Other);
	~CAnimator3D() override;
};

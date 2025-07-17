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
	// 애니메이션 목록
	map<wstring, Ptr<CAnimation>>		m_mapClip;

	const int		m_FPS;		// 30
	const float		m_FrameDuration;	// 1 / 30

	// 프레임 보간
	float			m_CurClipAccTime;
	float			m_CurClipRatio;
	float			m_NextClipAccTime;
	float			m_NextClipRatio;
	float			m_BlendAccTime;
	float			m_BlendDuration;
	float			m_BlendRatio;

	// 클립
	Ptr<CAnimation>			m_CurClip;			
	Ptr<CAnimation>			m_NextClip;

	// 프레임 인덱스
	int				m_CurClipCurFrameIdx;	// 현재 클립의 현재 프레임
	int				m_CurClipNextFrameIdx;	// 현재 클립의 다음 프레임
	int				m_NextClipCurFrameIdx;	// 다음 클립의 현재 프레임
	int				m_NextClipNextFrameIdx; // 다음 클립의 다음 프레임

	// Render
	CStructuredBuffer*	m_BoneFinalMatBuffer;	// 특정 프레임(보간)의 최종 행렬 (origin bone mat inverse * cur frame bone world mat)
	CStructuredBuffer*	m_BonePureMatBuffer;	// 특정 프레임(보간)의 bone 행렬 (origin bone mat inverse가 곱해지지 않은)

	bool				m_bFinalMatUpdate;		// 최종행렬 연산 수행여부 (Dirty Flag 역할)

	CMeshRender*		m_BindCaller;	// Bind를 호출한 MeshRenderer 기억

	// Bone에 대응되는 오브젝트를 저장
	vector<CGameObject*>	m_vecBoneObject;
	vector<Matrix>			m_vecBoneWorldTransform;

	// 활성화
	bool	m_Active;

public:
	void AddAnimClip(Ptr<CAnimation> _pAnim);
	void AddAnimClips(const vector<Ptr<CAnimation>>& _vecAnim);
	void SetAnimClips(const vector<Ptr<CAnimation>>& _vecAnim);
	Ptr<CAnimation> GetAnimClip(const wstring _AnimName);

	void ClearAnimClip() { m_mapClip.clear(); }

	Ptr<CAnimation> GetCurClip() const { return m_CurClip; }
	Ptr<CAnimation> GetNextClip() const { return m_NextClip; }

	int GetCurFrameIdx() const { return m_CurClipCurFrameIdx; }
	int GetNextFrameIdx() const { return m_NextClipCurFrameIdx; }
	float GetRatio() const { return m_CurClipRatio; }
	bool IsActive() const { return m_Active; }

	const map<wstring, Ptr<CAnimation>>& GetClips() const { return m_mapClip; }
	void SetCurClip(const wstring& _AnimName);
	void SetCurClipFrame(int _FrameIdx);

	// 애니메이션 Blending
	void SetCurClipBlend(const wstring& _AnimName, float _BlendTime);

	void Play() { m_Active = true; }
	void Pause() { m_Active = false; }

	UINT GetBoneCount() const { return m_CurClip->GetBoneCount(); }

	vector<CGameObject*> GetvecBone() { return m_vecBoneObject; }
	vector<Matrix> GetvecBoneWorldTrasnform() { return m_vecBoneWorldTransform; }

	CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
	void ClearData();

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

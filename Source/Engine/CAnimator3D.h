#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CAnimation.h"

class CStructuredBuffer;

class CAnimator3D :
    public CComponent
{
    vector<Ptr<CAnimation>>		m_vecClip;

    vector<float>	m_vecClipUpdateTime;
    vector<Matrix>	m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
    int				m_FrameCount; // 30
    double			m_CurTime;
    int				m_CurClip; // 클립 인덱스	

    int				m_FrameIdx; // 클립의 현재 프레임
    int				m_NextFrameIdx; // 클립의 다음 프레임
    float			m_Ratio; // 프레임 사이 비율

    CStructuredBuffer* m_BoneFinalMatBuffer; // 특정 프레임의 최종 행렬
    bool m_bFinalMatUpdate; // 최종행렬 연산 수행여부

public:
	void AddAnimClip(Ptr<CAnimation> _pAnim);
	void SetAnimClip(const vector<Ptr<CAnimation>>& _vecAnim);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

	void SetCurClip(int _Idx);

	UINT GetBoneCount() const { return m_vecClip[m_CurClip]->GetBoneCount(); }

    CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
    void ClearData(CMeshRender* _Renderer);

	const vector<tMTBone>* GetvecBone() { return m_vecBones; }
	const vector<tMTAnimClip>* GetvecClip() { return m_vecClip; }
	int GetCurClipIdx() { return m_CurClip; }
	int GetCurFrameIdx() { return m_FrameIdx; }
	double GetCurClipTime() { return m_CurTime; }
	float GetRatio() { return m_Ratio; }

    void Binding();


public:
    void FinalTick() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

    CLONE(CAnimator3D);
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _Other);
    ~CAnimator3D() override;
};

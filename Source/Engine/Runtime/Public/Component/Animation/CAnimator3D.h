#pragma once
#include "System/Public/Asset/Mesh/CMesh.h"
#include "Runtime/Public/Component/Base/CComponent.h"

class CStructuredBuffer;
class CMeshRender;

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

private:
	CMeshRender*	m_BindCaller;	// Bind를 호출한 MeshRenderer 기억

public:
	void AddAnimClip(Ptr<CAnimation> _pAnim);
	void SetAnimClip(const vector<Ptr<CAnimation>>& _vecAnim);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

	int GetCurClipIdx() const { return m_CurClip; }
	double GetCurClipTime() const { return m_CurTime; }
	int GetCurFrameIdx() const { return m_FrameIdx; }
	float GetRatio() const { return m_Ratio; }

	const vector<Ptr<CAnimation>>& GetClips() const { return m_vecClip; }

	void SetCurClip(int _Idx);

	UINT GetBoneCount() const { return m_vecClip[m_CurClip]->GetBoneCount(); }

    CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
    void ClearData();

    void Binding(CMeshRender* _Renderer);


public:
    void FinalTick() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

    CLONE(CAnimator3D);
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _Other);
    ~CAnimator3D() override;
};

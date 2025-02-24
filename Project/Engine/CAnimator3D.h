#pragma once
#include "CComponent.h"

#include "CMesh.h"

class CStructuredBuffer;

class CAnimator3D :
    public CComponent
{
    const vector<tMTBone>* m_vecBones;
    const vector<tMTAnimClip>* m_vecClip;

    vector<float> m_vecClipUpdateTime;
    vector<Matrix> m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
    int m_FrameCount; // 30
    double m_CurTime;
    int m_CurClip; // 클립 인덱스	

    int m_FrameIdx; // 클립의 현재 프레임
    int m_NextFrameIdx; // 클립의 다음 프레임
    float m_Ratio; // 프레임 사이 비율

    CStructuredBuffer* m_BoneFinalMatBuffer; // 특정 프레임의 최종 행렬
    bool m_bFinalMatUpdate; // 최종행렬 연산 수행여부


public:
    void SetBones(const vector<tMTBone>* _vecBones)
    {
        m_vecBones = _vecBones;
        m_vecFinalBoneMat.resize(m_vecBones->size());
    }

    void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

    CStructuredBuffer* GetFinalBoneMat() { return m_BoneFinalMatBuffer; }
    UINT GetBoneCount() { return static_cast<UINT>(m_vecBones->size()); }
    void ClearData();

    void Binding();

private:
    void check_mesh(Ptr<CMesh> _pMesh);

public:
    void FinalTick() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

    CLONE(CAnimator3D);
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _Other);
    ~CAnimator3D() override;
};

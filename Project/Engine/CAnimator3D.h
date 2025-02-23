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
    vector<Matrix> m_vecFinalBoneMat; // �ؽ��Ŀ� ������ ���� �������
    int m_FrameCount; // 30
    double m_CurTime;
    int m_CurClip; // Ŭ�� �ε���	

    int m_FrameIdx; // Ŭ���� ���� ������
    int m_NextFrameIdx; // Ŭ���� ���� ������
    float m_Ratio; // ������ ���� ����

    CStructuredBuffer* m_BoneFinalMatBuffer; // Ư�� �������� ���� ���
    bool m_bFinalMatUpdate; // ������� ���� ���࿩��


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

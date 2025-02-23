#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CBoneMatrixCS :
    public CComputeShader
{
    CStructuredBuffer* m_FrameDataBuffer; // t16
    CStructuredBuffer* m_OffsetMatBuffer; // t17 
    CStructuredBuffer* m_OutputBuffer; // u0

public:
    // g_int_0 : BonCount, g_int_1 : Frame Index
    void SetBoneCount(int _iBoneCount) { m_Const.iArr[0] = _iBoneCount; }
    void SetFrameIndex(int _iFrameIdx) { m_Const.iArr[1] = _iFrameIdx; }
    void SetNextFrameIdx(int _iFrameIdx) { m_Const.iArr[2] = _iFrameIdx; }
    void SetFrameRatio(float _fFrameRatio) { m_Const.fArr[0] = _fFrameRatio; }
    void SetFrameDataBuffer(CStructuredBuffer* _buffer) { m_FrameDataBuffer = _buffer; }
    void SetOffsetMatBuffer(CStructuredBuffer* _buffer) { m_OffsetMatBuffer = _buffer; }
    void SetOutputBuffer(CStructuredBuffer* _buffer) { m_OutputBuffer = _buffer; }

    int Binding() override;
    void CalcGroupCount() override;
    void Clear() override;

    CBoneMatrixCS();
    ~CBoneMatrixCS() override;
};

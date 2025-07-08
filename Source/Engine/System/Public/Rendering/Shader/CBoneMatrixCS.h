#pragma once
#include "System/Public/Rendering/Shader/CComputeShader.h"

class CStructuredBuffer;

class CBoneMatrixCS :
    public CComputeShader
{
private:
    CStructuredBuffer* m_OffsetMatBuffer; // t16
    CStructuredBuffer* m_CurClipFrameDataBuffer; // t17
	CStructuredBuffer* m_NextClipFrameDataBuffer; // t18
    CStructuredBuffer* m_OutputBuffer; // u0
	CStructuredBuffer* m_PureOutputBuffer; // u1

public:
    void SetBoneCount(int _iBoneCount) { m_Const.iArr[0] = _iBoneCount; }
    void SetCurClipFrame(float _Frame) { m_Const.fArr[0] = _Frame; }
	void SetNextClipFrame(float _Frame) { m_Const.fArr[1] = _Frame; }
	void SetBlendRatio(float _Ratio) { m_Const.fArr[2] = _Ratio; }
    void SetCurClipFrameBuffer(CStructuredBuffer* _buffer) { m_CurClipFrameDataBuffer = _buffer; }
	void SetNextClipFrameBuffer(CStructuredBuffer* _buffer) { m_NextClipFrameDataBuffer = _buffer; }
    void SetOffsetMatBuffer(CStructuredBuffer* _buffer) { m_OffsetMatBuffer = _buffer; }
    void SetOutputBuffer(CStructuredBuffer* _buffer) { m_OutputBuffer = _buffer; }
	void SetPureOutputBuffer(CStructuredBuffer* _buffer) { m_PureOutputBuffer = _buffer; }

    int Binding() override;
    void CalculateGroupCount() override;
    void Clear() override;

    CBoneMatrixCS();
    ~CBoneMatrixCS() override;
};

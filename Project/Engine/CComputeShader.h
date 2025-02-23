#pragma once
#include "CShader.h"

#include "Ptr.h"
#include "CTexture.h"

class CComputeShader :
    public CShader
{
    ComPtr<ID3DBlob> m_CSBlob; // HLSL �ڵ带 �������� ��� ����
    ComPtr<ID3D11ComputeShader> m_CS; // ��ǻƮ ���̴� ��ü

protected:
    MtrlConst m_Const;

    const int m_GroupPerThreadX;
    const int m_GroupPerThreadY;
    const int m_GroupPerThreadZ;

    int m_GroupX;
    int m_GroupY;
    int m_GroupZ;

private:
    int CreateComputeShader(const wstring& _RelativePath, const string& _FuncName);

public:
    int Execute();

private:
    virtual int Binding() = 0;
    virtual void CalcGroupCount() = 0;
    virtual void Clear() = 0;

public:
    CLONE_DISABLE(CComputeShader);
    CComputeShader(const wstring& _RelativeFilePath, const string& _FuncName
                   , int _GroupPerX, int _GroupPerY, int _GroupPerZ);
    ~CComputeShader() override;
};

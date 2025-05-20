#pragma once
#include "Engine/System/Public/Rendering/Shader/CShader.h"

struct tScalarParam
{
    string Desc;
    SCALAR_PARAM eParam;
    bool Drag;
};

struct tTexParam
{
    string Desc;
    TEX_PARAM eParam;
};


class CGraphicShader :
    public CShader
{
    ComPtr<ID3DBlob> m_VSBlob;
	ComPtr<ID3DBlob> m_VSInstBlob;
    ComPtr<ID3DBlob> m_HSBlob;
    ComPtr<ID3DBlob> m_DSBlob;
    ComPtr<ID3DBlob> m_GSBlob;
    ComPtr<ID3DBlob> m_PSBlob;

    ComPtr<ID3D11VertexShader> m_VS;
	ComPtr<ID3D11VertexShader> m_VSInst;
    ComPtr<ID3D11HullShader> m_HS;
    ComPtr<ID3D11DomainShader> m_DS;
    ComPtr<ID3D11GeometryShader> m_GS;
    ComPtr<ID3D11PixelShader> m_PS;

    ComPtr<ID3D11InputLayout> m_Layout;
	ComPtr<ID3D11InputLayout> m_LayoutInst;
    D3D11_PRIMITIVE_TOPOLOGY m_Topology;

    RS_TYPE m_RSType; // 레스터라이져 스테이트 옵션
    BS_TYPE m_BSType; // 블랜드 스테이트 옵션
    DS_TYPE m_DSType; // DepthStencilState 옵션
    UINT m_StencilRef;


    SHADER_DOMAIN m_Domain;

    vector<tScalarParam> m_vecScalarParam;
    vector<tTexParam> m_vecTexParam;

public:
    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateHullShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateDomainShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);

    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void SetRSState(RS_TYPE _Type) { m_RSType = _Type; }
    void SetBSState(BS_TYPE _Type) { m_BSType = _Type; }
    void SetDSState(DS_TYPE _Type) { m_DSType = _Type; }

    void SetStencilRef(UINT _Stencil) { m_StencilRef = _Stencil; }

    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }

    void AddScalarParam(const string& _Desc, SCALAR_PARAM _Param, bool _Drag = false)
    {
        m_vecScalarParam.push_back(tScalarParam{_Desc, _Param, _Drag});
    }

    void AddTexParam(const string& _Desc, TEX_PARAM _Param)
    {
        m_vecTexParam.push_back(tTexParam{_Desc, _Param});
    }

    const vector<tScalarParam>& GetScalarParam() { return m_vecScalarParam; }
    const vector<tTexParam>& GetTexParam() { return m_vecTexParam; }


    void Binding();
	void Binding_Inst();

	ComPtr<ID3D11VertexShader> GetVSInst() { return m_VSInst; }

private:
	int CreateInputLayout();

public:
    CLONE_DISABLE(CGraphicShader);
    CGraphicShader();
    ~CGraphicShader() override;
};

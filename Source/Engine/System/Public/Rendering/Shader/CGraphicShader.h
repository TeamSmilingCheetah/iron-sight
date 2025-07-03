#pragma once
#include "Engine/System/Public/Rendering/Shader/CShader.h"

struct tScalarParam;

class CGraphicShader :
	public CShader
{
private:
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

private:
	int CreateInputLayout();

public:
	void Binding();
	void BindingWithInstancing();

	int CreateVertexShader(const wstring& PBlobFilePath,
	                       const wstring& PEffectsFilePath, const wstring& PEntryPointName);
	int CreateHullShader(const wstring& PBlobFilePath,
	                     const wstring& PEffectsFilePath, const wstring& PEntryPointName);
	int CreateDomainShader(const wstring& PBlobFilePath,
	                       const wstring& PEffectsFilePath, const wstring& PEntryPointName);
	int CreateGeometryShader(const wstring& PBlobFilePath,
	                         const wstring& PEffectsFilePath, const wstring& PEntryPointName);
	int CreatePixelShader(const wstring& PBlobFilePath,
	                      const wstring& PEffectsFilePath, const wstring& PEntryPointName);

	void AddScalarParam(const string& PDesc, SCALAR_PARAM PParam, bool PDrag = false)
	{
		m_vecScalarParam.push_back(tScalarParam{PDesc, PParam, PDrag});
	}

	void AddTexParam(const string& PDesc, TEX_PARAM PParam)
	{
		m_vecTexParam.push_back(tTexParam{PDesc, PParam});
	}

	// Getter & Setter
	SHADER_DOMAIN GetDomain() const { return m_Domain; }
	const vector<tScalarParam>& GetScalarParam() { return m_vecScalarParam; }
	const vector<tTexParam>& GetTexParam() { return m_vecTexParam; }
	ComPtr<ID3D11VertexShader> GetVSInst() { return m_VSInst; }

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY PTopology) { m_Topology = PTopology; }
	void SetRSState(RS_TYPE PType) { m_RSType = PType; }
	void SetBSState(BS_TYPE PType) { m_BSType = PType; }
	void SetDSState(DS_TYPE PType) { m_DSType = PType; }
	void SetStencilRef(UINT PStencil) { m_StencilRef = PStencil; }
	void SetDomain(SHADER_DOMAIN PDomain) { m_Domain = PDomain; }

	// Special Member Function
	CLONE_DISABLE(CGraphicShader);
	CGraphicShader();
	~CGraphicShader() override;
};

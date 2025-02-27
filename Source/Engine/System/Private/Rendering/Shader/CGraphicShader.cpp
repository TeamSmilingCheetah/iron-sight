#include "pch.h"
#include "System/Public/Rendering/Shader/CGraphicShader.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"

CGraphicShader::CGraphicShader()
	: CShader(GRAPHIC_SHADER)
	  , m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	  , m_RSType(RS_TYPE::CULL_BACK)
	  , m_BSType(BS_TYPE::DEFAULT)
	  , m_DSType(DS_TYPE::LESS)
	  , m_StencilRef(0)
	  , m_Domain(SHADER_DOMAIN::DOMAIN_NONE)
{
}

CGraphicShader::~CGraphicShader()
{
}

int CGraphicShader::CreateVertexShader(const wstring& _RelativePath, const string& _FuncName)
{
	// 쉐이더 컴파일
	wstring SourcePath = CPathMgr::GetInst()->GetSrcPath();
	UINT Flag = D3DCOMPILE_DEBUG;
	HRESULT hr = S_OK;

	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , _FuncName.c_str(), "vs_5_0", Flag, 0
	                        , m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());
	if (FAILED(hr))
	{
		errno_t errNum = GetLastError();

		if (2 == errNum || 3 == errNum)
		{
			// 잘못된 경로
			MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}

		else
		{
			auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
	                           , m_VSBlob->GetBufferSize()
	                           , nullptr, m_VS.GetAddressOf());

	// 추가 버텍스쉐이더(Instancing 버전) 컴파일
	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , (_FuncName + "_Inst").c_str(), "vs_5_0", Flag, 0
	                        , m_VSInstBlob.GetAddressOf()
	                        , m_ErrBlob.GetAddressOf());

	// 추가 인스턴싱 VS 가 존재한다면
	if (S_OK == hr)
	{
		// 컴파일 된 코드로 Vertex Instancing Shader 객체 만들기
		DEVICE->CreateVertexShader(m_VSInstBlob->GetBufferPointer()
		                           , m_VSInstBlob->GetBufferSize()
		                           , nullptr, m_VSInst.GetAddressOf());
	}

	if (FAILED(CreateInputLayout()))
	{
		assert(nullptr);
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreateHullShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring SourcePath = CPathMgr::GetInst()->GetSrcPath();

	HRESULT hr = S_OK;
	UINT Flag = D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , _FuncName.c_str(), "hs_5_0", Flag, 0
	                        , m_HSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		errno_t errNum = GetLastError();

		if (2 == errNum || 3 == errNum)
		{
			// 잘못된 경로
			MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}

		else
		{
			auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer()
	                         , m_HSBlob->GetBufferSize()
	                         , nullptr, m_HS.GetAddressOf());

	return S_OK;
}

int CGraphicShader::CreateDomainShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring SourcePath = CPathMgr::GetInst()->GetSrcPath();

	HRESULT hr = S_OK;
	UINT Flag = D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , _FuncName.c_str(), "ds_5_0", Flag, 0
	                        , m_DSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		errno_t errNum = GetLastError();

		if (2 == errNum || 3 == errNum)
		{
			// 잘못된 경로
			MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}

		else
		{
			auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer()
	                           , m_DSBlob->GetBufferSize()
	                           , nullptr, m_DS.GetAddressOf());

	return S_OK;
}

int CGraphicShader::CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring SourcePath = CPathMgr::GetInst()->GetSrcPath();

	HRESULT hr = S_OK;
	UINT Flag = D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , _FuncName.c_str(), "gs_5_0", Flag, 0
	                        , m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		errno_t errNum = GetLastError();

		if (2 == errNum || 3 == errNum)
		{
			// 잘못된 경로
			MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}

		else
		{
			auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer()
	                             , m_GSBlob->GetBufferSize()
	                             , nullptr, m_GS.GetAddressOf());

	return S_OK;
}

int CGraphicShader::CreatePixelShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring SourcePath = CPathMgr::GetInst()->GetSrcPath();

	HRESULT hr = S_OK;
	UINT Flag = D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(wstring(SourcePath + _RelativePath).c_str()
	                        , nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	                        , _FuncName.c_str(), "ps_5_0", Flag, 0
	                        , m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());
	if (FAILED(hr))
	{
		errno_t errNum = GetLastError();

		if (2 == errNum || 3 == errNum)
		{
			// 잘못된 경로
			MessageBoxA(nullptr, "쉐이더 파일이 존재하지 않습니다.", "쉐이더 컴파일 실패", MB_OK);
		}

		else
		{
			auto pErrMsg = static_cast<char*>(m_ErrBlob->GetBufferPointer());
			MessageBoxA(nullptr, pErrMsg, "쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
	                          , m_PSBlob->GetBufferSize()
	                          , nullptr, m_PS.GetAddressOf());

	return S_OK;
}


void CGraphicShader::Binding()
{
	CONTEXT->IASetPrimitiveTopology(m_Topology); // 도형의 위상(위치, 상태)
	CONTEXT->IASetInputLayout(m_Layout.Get());

	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	// Rstareizer 바인딩
	ComPtr<ID3D11RasterizerState> pRSState = CDevice::GetInst()->GetRSState(m_RSType);
	CONTEXT->RSSetState(pRSState.Get());

	// BlendState 바인딩
	ComPtr<ID3D11BlendState> pBSState = CDevice::GetInst()->GetBSState(m_BSType);
	CONTEXT->OMSetBlendState(pBSState.Get(), nullptr, 0xffffffff);

	// DepthStencilState 바인딩
	ComPtr<ID3D11DepthStencilState> pDSState = CDevice::GetInst()->GetDSState(m_DSType);
	CONTEXT->OMSetDepthStencilState(pDSState.Get(), m_StencilRef);
}

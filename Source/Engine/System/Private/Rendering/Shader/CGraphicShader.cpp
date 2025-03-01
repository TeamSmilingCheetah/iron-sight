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

void CGraphicShader::Binding_Inst()
{
	CONTEXT->IASetPrimitiveTopology(m_Topology); // 도형의 위상(위치, 상태)
	CONTEXT->IASetInputLayout(m_LayoutInst.Get());

	CONTEXT->VSSetShader(m_VSInst.Get(), nullptr, 0);
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

int CGraphicShader::CreateInputLayout()
{
	// InputLayout
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[21] = {};

	LayoutDesc[0].AlignedByteOffset = 0;
	LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[0].InputSlot = 0;
	LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[0].InstanceDataStepRate = 0;
	LayoutDesc[0].SemanticName = "POSITION";
	LayoutDesc[0].SemanticIndex = 0;

	LayoutDesc[1].AlignedByteOffset = 12;
	LayoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	LayoutDesc[1].InputSlot = 0;
	LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[1].InstanceDataStepRate = 0;
	LayoutDesc[1].SemanticName = "TEXCOORD";
	LayoutDesc[1].SemanticIndex = 0;

	LayoutDesc[2].AlignedByteOffset = 20;
	LayoutDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	LayoutDesc[2].InputSlot = 0;
	LayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[2].InstanceDataStepRate = 0;
	LayoutDesc[2].SemanticName = "COLOR";
	LayoutDesc[2].SemanticIndex = 0;

	LayoutDesc[3].AlignedByteOffset = 36;
	LayoutDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[3].InputSlot = 0;
	LayoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[3].InstanceDataStepRate = 0;
	LayoutDesc[3].SemanticName = "TANGENT";
	LayoutDesc[3].SemanticIndex = 0;

	LayoutDesc[4].AlignedByteOffset = 48;
	LayoutDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[4].InputSlot = 0;
	LayoutDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[4].InstanceDataStepRate = 0;
	LayoutDesc[4].SemanticName = "NORMAL";
	LayoutDesc[4].SemanticIndex = 0;

	LayoutDesc[5].AlignedByteOffset = 60;
	LayoutDesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[5].InputSlot = 0;
	LayoutDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[5].InstanceDataStepRate = 0;
	LayoutDesc[5].SemanticName = "BINORMAL";
	LayoutDesc[5].SemanticIndex = 0;

	LayoutDesc[6].AlignedByteOffset = 72;
	LayoutDesc[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	LayoutDesc[6].InputSlot = 0;
	LayoutDesc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[6].InstanceDataStepRate = 0;
	LayoutDesc[6].SemanticName = "BLENDWEIGHT";
	LayoutDesc[6].SemanticIndex = 0;

	LayoutDesc[7].AlignedByteOffset = 88;
	LayoutDesc[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	LayoutDesc[7].InputSlot = 0;
	LayoutDesc[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[7].InstanceDataStepRate = 0;
	LayoutDesc[7].SemanticName = "BLENDINDICES";
	LayoutDesc[7].SemanticIndex = 0;

	// VS 용 InputLayout 생성
	if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 8
		, m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 두번째 슬롯
	if (nullptr != m_VSInstBlob)
	{
		LayoutDesc[8].SemanticName = "WORLD";
		LayoutDesc[8].SemanticIndex = 0;
		LayoutDesc[8].AlignedByteOffset = 0;
		LayoutDesc[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[8].InputSlot = 1;
		LayoutDesc[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[8].InstanceDataStepRate = 1;

		LayoutDesc[9].SemanticName = "WORLD";
		LayoutDesc[9].SemanticIndex = 1;
		LayoutDesc[9].AlignedByteOffset = 16;
		LayoutDesc[9].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[9].InputSlot = 1;
		LayoutDesc[9].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[9].InstanceDataStepRate = 1;

		LayoutDesc[10].SemanticName = "WORLD";
		LayoutDesc[10].SemanticIndex = 2;
		LayoutDesc[10].AlignedByteOffset = 32;
		LayoutDesc[10].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[10].InputSlot = 1;
		LayoutDesc[10].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[10].InstanceDataStepRate = 1;

		LayoutDesc[11].SemanticName = "WORLD";
		LayoutDesc[11].SemanticIndex = 3;
		LayoutDesc[11].AlignedByteOffset = 48;
		LayoutDesc[11].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[11].InputSlot = 1;
		LayoutDesc[11].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[11].InstanceDataStepRate = 1;

		LayoutDesc[12].SemanticName = "WV";
		LayoutDesc[12].SemanticIndex = 0;
		LayoutDesc[12].AlignedByteOffset = 64;
		LayoutDesc[12].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[12].InputSlot = 1;
		LayoutDesc[12].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[12].InstanceDataStepRate = 1;

		LayoutDesc[13].SemanticName = "WV";
		LayoutDesc[13].SemanticIndex = 1;
		LayoutDesc[13].AlignedByteOffset = 80;
		LayoutDesc[13].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[13].InputSlot = 1;
		LayoutDesc[13].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[13].InstanceDataStepRate = 1;

		LayoutDesc[14].SemanticName = "WV";
		LayoutDesc[14].SemanticIndex = 2;
		LayoutDesc[14].AlignedByteOffset = 96;
		LayoutDesc[14].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[14].InputSlot = 1;
		LayoutDesc[14].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[14].InstanceDataStepRate = 1;

		LayoutDesc[15].SemanticName = "WV";
		LayoutDesc[15].SemanticIndex = 3;
		LayoutDesc[15].AlignedByteOffset = 112;
		LayoutDesc[15].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[15].InputSlot = 1;
		LayoutDesc[15].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[15].InstanceDataStepRate = 1;

		LayoutDesc[16].SemanticName = "WVP";
		LayoutDesc[16].SemanticIndex = 0;
		LayoutDesc[16].AlignedByteOffset = 128;
		LayoutDesc[16].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[16].InputSlot = 1;
		LayoutDesc[16].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[16].InstanceDataStepRate = 1;

		LayoutDesc[17].SemanticName = "WVP";
		LayoutDesc[17].SemanticIndex = 1;
		LayoutDesc[17].AlignedByteOffset = 144;
		LayoutDesc[17].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[17].InputSlot = 1;
		LayoutDesc[17].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[17].InstanceDataStepRate = 1;

		LayoutDesc[18].SemanticName = "WVP";
		LayoutDesc[18].SemanticIndex = 2;
		LayoutDesc[18].AlignedByteOffset = 160;
		LayoutDesc[18].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[18].InputSlot = 1;
		LayoutDesc[18].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[18].InstanceDataStepRate = 1;

		LayoutDesc[19].SemanticName = "WVP";
		LayoutDesc[19].SemanticIndex = 3;
		LayoutDesc[19].AlignedByteOffset = 176;
		LayoutDesc[19].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		LayoutDesc[19].InputSlot = 1;
		LayoutDesc[19].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[19].InstanceDataStepRate = 1;

		LayoutDesc[20].SemanticName = "ROWINDEX";
		LayoutDesc[20].SemanticIndex = 0;
		LayoutDesc[20].AlignedByteOffset = 192;
		LayoutDesc[20].Format = DXGI_FORMAT_R32_UINT;
		LayoutDesc[20].InputSlot = 1;
		LayoutDesc[20].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		LayoutDesc[20].InstanceDataStepRate = 1;

		if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 21
			, m_VSInstBlob->GetBufferPointer()
			, m_VSInstBlob->GetBufferSize()
			, m_LayoutInst.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

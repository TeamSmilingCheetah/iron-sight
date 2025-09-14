#include "pch.h"
#include "System/Public/Asset/Texture/CTexture.h"
#include "System/Public/Manager/CPathMgr.h"
#include "System/Public/Rendering/Device/CDevice.h"

CTexture::CTexture()
	: FAsset(TEXTURE, false)
	, m_Desc{}
	, m_RecentSRVNum(-1)
	, m_RecentUAVNum(-1)
	, m_IsMemoryReleased(false)
{
}

CTexture::~CTexture() = default;

int CTexture::Load(const wstring& PFilePath)
{
	// 파일 -> SystemMem
	wchar_t szExt[50] = {};
	_wsplitpath_s(PFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);
	wstring strExt = szExt;

	HRESULT hr;

	TexMetadata MetaData;

	if (strExt == L".dds" || strExt == L".DDS")
	{
		// DDS
		hr = LoadFromDDSFile(PFilePath.c_str(), DDS_FLAGS_NONE, &MetaData, m_Image);
	}
	else if (strExt == L".tga" || strExt == L".TGA")
	{
		// TGA
		hr = LoadFromTGAFile(PFilePath.c_str(), &MetaData, m_Image);
	}
	else
	{
		// Window Image Component(WIC)
		// png, jpg, jpeg, bmp
		hr = LoadFromWICFile(PFilePath.c_str(), WIC_FLAGS_NONE, &MetaData, m_Image);
	}

	if (FAILED(hr))
	{
		wstring message = L"텍스쳐 로딩 실패\n: ";
		message += PFilePath;
		MessageBox(nullptr, message.c_str(), L"리소스 로딩 실패", MB_OK);
		return E_FAIL;
	}

	// System -> GPU
	// m_Image -> m_Tex2D
	// Texture2D 객체를 만듬
	// Texture2D 를 전달할때 사용할 ShaderResourceView
	//CreateShaderResourceView(DEVICE
	//                         , m_Image.GetImages()
	//                         , m_Image.GetImageCount()
	//                         , m_Image.GetMetadata()
	//                         , m_SRV.GetAddressOf());

	//// 생성된 ShaderResourceView 를 이용해서 원본 객체(Texture2D) 를 알아낸다.
	//m_SRV->GetResource(reinterpret_cast<ID3D11Resource**>(m_Tex2D.GetAddressOf()));

	//// 생성된 Texture2D 의 Desc 정보를 알아낸다.
	//m_Tex2D->GetDesc(&m_Desc);

	// ScratchImage의 메타데이터
	DXGI_FORMAT SRVFormat = MetaData.format;

	// Texture에 세팅된 메타데이터 (세부 옵션)
	wstring MetaFilePath = PFilePath + L".meta";
	if (FAILED(LoadMetaOpts(MetaFilePath)))
	{
		// 메타데이터 파일이 없는 경우, 기본 값으로 로드
		// typeless로 import, 기본적으로 sRGB를 사용하지 않는 옵션 채택
		m_MetaOpts.sRGB = false;
	
		// 마지막에 세팅된 메타데이터를 저장
		SaveMetaOpts(MetaFilePath);
	}

	MetaData.format = GetTypeLessFormat(MetaData.format);
	SRVFormat = GetShaderResourceFormat(MetaData.format, m_MetaOpts.sRGB);

	// TODO(Ssio) : mipmap도 옵션으로 설계
	if (MetaData.mipLevels > 1 || MetaData.IsCubemap())
	{
		// SRV 생성 (DirectX Tex 함수)
		//if (FAILED(CreateShaderResourceView(DEVICE, m_Image.GetImages(), m_Image.GetImageCount(), MetaData, m_SRV.GetAddressOf())))
		//{
		//	return E_FAIL;
		//}

		// Texture2D 생성
		m_Desc = {};
		m_Desc.Width = static_cast<UINT>(MetaData.width);
		m_Desc.Height = static_cast<UINT>(MetaData.height);
		m_Desc.MipLevels = static_cast<UINT>(MetaData.mipLevels);
		m_Desc.ArraySize = static_cast<UINT>(MetaData.arraySize);
		m_Desc.Format = MetaData.format;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_Desc.CPUAccessFlags = 0;
		m_Desc.MiscFlags = MetaData.miscFlags;

		// Subresource 채우기
		const Image* imgs = m_Image.GetImages();
		const size_t n = m_Image.GetImageCount();
		vector<D3D11_SUBRESOURCE_DATA> init(n);
		for (size_t i = 0; i < n; ++i)
		{
			init[i].pSysMem = imgs[i].pixels;
			init[i].SysMemPitch = static_cast<UINT>(imgs[i].rowPitch);
			init[i].SysMemSlicePitch = static_cast<UINT>(imgs[i].slicePitch);
		}

		if (FAILED(DEVICE->CreateTexture2D(&m_Desc, init.data(), m_Tex2D.GetAddressOf())))
		{
			return E_FAIL;
		}

		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetShaderResourceFormat(m_Desc.Format, m_MetaOpts.sRGB);
		srvDesc.ViewDimension = MetaData.IsCubemap() ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &srvDesc, &m_SRV)))
		{
			return E_FAIL;
		}
	}

	// MipMap이 포함되어 있지 않다면
	else
	{
		// MipMap 생성
		m_Desc = {};
		m_Desc.Width = static_cast<UINT>(MetaData.width);
		m_Desc.Height = static_cast<UINT>(MetaData.height);
		m_Desc.MipLevels = 0; // 자동 생성
		m_Desc.ArraySize = 1;
		m_Desc.Format = MetaData.format;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	// 왜인지 모르겠으나 render target도 해줘야 함
		m_Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		// 텍스쳐를 생성 (주의 : 생성할 때 리소스를 넘겨주지 않는다!!)
		if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, &m_Tex2D)))
		{
			return E_FAIL;
		}

		// ShaderResourceView 생성 (mip level 전체 범위 지정)
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetShaderResourceFormat(m_Desc.Format, m_MetaOpts.sRGB);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &srvDesc, &m_SRV)))
		{
			return E_FAIL;
		}

		// 원본 이미지 업로드 (0번 mip level만)
		CONTEXT->UpdateSubresource(m_Tex2D.Get(), 0, nullptr, m_Image.GetImages()->pixels,
			static_cast<UINT>(m_Image.GetImages()->rowPitch), static_cast<UINT>(m_Image.GetImages()->slicePitch));

		// Mipmap 자동 생성 (0번 텍스쳐를 기반으로 mipmap을 생성함)
		CONTEXT->GenerateMips(m_SRV.Get());
	}

	// Release Memory
	ReleaseSystemMemory();

	return S_OK;
}

int CTexture::Save(const wstring& _RelativePath)
{
	// GPU -> System
	ScratchImage localImage;
	CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), localImage);

	// System -> File
	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;

	HRESULT hr;
	if (1 == localImage.GetMetadata().arraySize)
	{
		// png, jpg, jpeg, bmp,
		hr = SaveToWICFile(*localImage.GetImages()
		                   , WIC_FLAGS_NONE
		                   , GetWICCodec(WIC_CODEC_PNG)
		                   , strFilePath.c_str());
	}

	else
	{
		hr = SaveToDDSFile(localImage.GetImages()
		                   , localImage.GetMetadata().arraySize
		                   , localImage.GetMetadata()
		                   , DDS_FLAGS_NONE
		                   , strFilePath.c_str());
	}

	SaveMetaOpts(strFilePath + L".meta");

	return hr;
}

int CTexture::Create(UINT PWidth, UINT PHeight, DXGI_FORMAT PPixelFormat, UINT PBindFlag,
                     D3D11_USAGE PUsage)
{
	m_Desc.Width = PWidth;
	m_Desc.Height = PHeight;

	m_Desc.ArraySize = 1;
	m_Desc.Format = PPixelFormat;

	// 텍스쳐의 용도
	m_Desc.BindFlags = PBindFlag;

	// CPU 에서 생성 이후에 접근이 가능한지 옵션
	m_Desc.Usage = PUsage;

	if (m_Desc.Usage == D3D11_USAGE_DYNAMIC)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (m_Desc.Usage == D3D11_USAGE_STAGING)
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	// 밉맵 개수가 1 ==> 원본만 존재함
	m_Desc.MipLevels = 1;

	m_Desc.MiscFlags = 0;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	// View 생성
	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(
				DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(
				DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			if (FAILED(
				DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

int CTexture::Create(ComPtr<ID3D11Texture2D> P2DTexture)
{
	assert(!m_Tex2D.Get());

	m_Tex2D = P2DTexture;
	m_Tex2D->GetDesc(&m_Desc);

	// View 생성
	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(
				DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}

		if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (FAILED(
				DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf())))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

int CTexture::CreateArrayTexture(const vector<Ptr<CTexture>>& PTextureVector)
{
	m_Desc = PTextureVector[0]->GetDesc();
	m_Desc.ArraySize = static_cast<UINT>(PTextureVector.size());
	m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_Desc.MipLevels = 1;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 원본 각 텍스쳐를 생성된 배열 텍스쳐의 각 칸으로 복사시킨다.
	for (size_t i = 0; i < PTextureVector.size(); ++i)
	{
		UINT Offset = D3D11CalcSubresource(0, static_cast<UINT>(i), 1);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), Offset, nullptr
		                           , PTextureVector[i]->GetPixels()
		                           , static_cast<UINT>(PTextureVector[i]->GetRowPitch())
		                           , static_cast<UINT>(PTextureVector[i]->GetSlicePitch()));
	}

	// Shader Resrouce View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.Format = m_Desc.Format;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tSRVDesc.Texture2DArray.MipLevels = 1;
	tSRVDesc.Texture2DArray.MostDetailedMip = 0;
	tSRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;

	if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

int CTexture::LoadMetaOpts(const wstring& PFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, PFilePath.c_str(), L"rb");

	// Meta 파일이 없는 경우
	if (pFile == nullptr)
	{
		return E_FAIL;
	}

	fread(&m_MetaOpts.sRGB, sizeof(bool), 1, pFile);
	fread(&m_MetaOpts.mipGen, sizeof(bool), 1, pFile);

	fclose(pFile);

	return S_OK;
}

int CTexture::SaveMetaOpts(const wstring& PFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, PFilePath.c_str(), L"wb");

	fwrite(&m_MetaOpts.sRGB, sizeof(bool), 1, pFile);
	fwrite(&m_MetaOpts.mipGen, sizeof(bool), 1, pFile);

	fclose(pFile);

	return S_OK;
}

DXGI_FORMAT CTexture::GetTypeLessFormat(DXGI_FORMAT InFormat)
{
	switch (InFormat)
	{
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return DXGI_FORMAT_BC1_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return DXGI_FORMAT_BC2_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return DXGI_FORMAT_BC3_TYPELESS;
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return DXGI_FORMAT_BC7_TYPELESS;
	};

	return InFormat;
}

DXGI_FORMAT CTexture::GetShaderResourceFormat(DXGI_FORMAT InFormat, bool bSRGB)
{
	if (bSRGB)
	{
		switch (InFormat)
		{
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_BC1_TYPELESS:         return DXGI_FORMAT_BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_TYPELESS:         return DXGI_FORMAT_BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_TYPELESS:         return DXGI_FORMAT_BC3_UNORM_SRGB;
		case DXGI_FORMAT_BC7_TYPELESS:         return DXGI_FORMAT_BC7_UNORM_SRGB;
		};
	}
	else
	{
		switch (InFormat)
		{
		case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:      return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_TYPELESS:      return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_TYPELESS:      return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC7_TYPELESS:      return DXGI_FORMAT_BC7_UNORM;
		};
	}

	switch (InFormat)
	{
	case DXGI_FORMAT_R24G8_TYPELESS:    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case DXGI_FORMAT_R32_TYPELESS:      return DXGI_FORMAT_R32_FLOAT;
	case DXGI_FORMAT_R16_TYPELESS:      return DXGI_FORMAT_R16_UNORM;
	case DXGI_FORMAT_R32G8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	}

	return InFormat;
}

bool CTexture::CanUseSRGB(DXGI_FORMAT InFormat)
{
	switch (InFormat)
	{
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC7_TYPELESS:
		return true;
	}

	return false;
}

int CTexture::GenerateMip(UINT PLevel)
{
	// CubeTexture 는 Mipmap 생성 금지
	assert(!(m_Desc.MiscFlags & D3D11_SRV_DIMENSION_TEXTURECUBE));

	m_Tex2D = nullptr;
	m_RTV = nullptr;
	m_DSV = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;

	m_Desc.MipLevels = PLevel;
	m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	for (UINT i = 0; i < m_Desc.ArraySize; ++i)
	{
		UINT iSubIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubIdx, nullptr
		                           , m_Image.GetImage(0, i, 0)->pixels
		                           , static_cast<UINT>(m_Image.GetImage(0, i, 0)->rowPitch)
		                           , static_cast<UINT>(m_Image.GetImage(0, i, 0)->slicePitch));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	if (2 <= m_Desc.ArraySize)
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
		SRVDesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
		SRVDesc.Texture2DArray.MostDetailedMip = 0;
	}
	else
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = m_Desc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
	}

	if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &SRVDesc, m_SRV.GetAddressOf())))
		return E_FAIL;

	CONTEXT->GenerateMips(m_SRV.Get());

	return S_OK;
}

void CTexture::Binding(int PRegisterNum)
{
	CONTEXT->VSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Clear(int PRegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(PRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(PRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(PRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(PRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(PRegisterNum, 1, &pSRV);
}

void CTexture::Binding_SRV_CS(int PRegisterNum)
{
	m_RecentSRVNum = PRegisterNum;
	CONTEXT->CSSetShaderResources(PRegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Binding_UAV_CS(int PRegisterNum)
{
	assert(m_UAV.Get());

	m_RecentUAVNum = PRegisterNum;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(PRegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void CTexture::Clear_SRV_CS()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(m_RecentSRVNum, 1, &pSRV);
}

void CTexture::Clear_UAV_CS()
{
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_RecentUAVNum, 1, &pUAV, &i);
}

void CTexture::SetMetaOpts(const MetaOpts& POpts)
{
	// Option을 하나 하나 비교하면서 변경사항이 있으면 적용하기

	bool DirtyFlag = false;

	// 텍스쳐 및 SRV 재생성 여부
	bool Regenerate = false;

	// SRGB
	if (CanUseSRGB(m_Desc.Format))
	{
		if (m_MetaOpts.sRGB != POpts.sRGB)
		{
			DXGI_FORMAT SRVFormat = GetShaderResourceFormat(m_Desc.Format, POpts.sRGB);

			if (SRVFormat != m_Desc.Format)
			{
				m_MetaOpts.sRGB = POpts.sRGB;
				Regenerate = true;
				DirtyFlag = true;
			}
		}
	}
	else
	{
		LOG_INFO("SRGB is not supported with this texture format.");
	}

	// mipmap
	if (m_MetaOpts.mipGen != POpts.mipGen)
	{
		Regenerate = true;
		DirtyFlag = true;

		if (POpts.mipGen)
		{

		}

		else
		{

		}
	}

	// SRV 재생성
	if (Regenerate)
	{
		// ShaderResourceView 생성 (mip level 전체 범위 지정)
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetShaderResourceFormat(m_Desc.Format, m_MetaOpts.sRGB);
		srvDesc.ViewDimension = m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		m_SRV = nullptr;
		if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &srvDesc, &m_SRV)))
		{
			assert(!"Failed regenerating texture SRV!");
		}
	}

	if (DirtyFlag)
	{
		wstring FilePath = CPathMgr::GetInst()->GetContentPath() + GetKey() + L".meta";
		SaveMetaOpts(FilePath);
	}
}

tPixel* CTexture::GetPixels()
{
	if (m_IsMemoryReleased)
	{
		// Recapture If Released
		CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
		m_IsMemoryReleased = false;
	}
	return reinterpret_cast<tPixel*>(m_Image.GetPixels());
}

void CTexture::ReleaseSystemMemory()
{
	if (!m_IsMemoryReleased)
	{
		m_Image = ScratchImage();
		m_IsMemoryReleased = true;
	}
}

const TexMetadata& CTexture::GetMetaData()
{
	if (m_IsMemoryReleased)
	{
		// Recapture If Released
		CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
		m_IsMemoryReleased = false;
	}
	return m_Image.GetMetadata();
}

size_t CTexture::GetRowPitch()
{
	if (m_IsMemoryReleased)
	{
		// Recapture If Released
		CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
		m_IsMemoryReleased = false;
	}
	return m_Image.GetImages()->rowPitch;
}

size_t CTexture::GetSlicePitch()
{
	if (m_IsMemoryReleased)
	{
		// Recapture If Released
		CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
		m_IsMemoryReleased = false;
	}
	return m_Image.GetImages()->slicePitch;
}

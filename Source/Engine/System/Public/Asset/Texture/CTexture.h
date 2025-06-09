#pragma once
#include "Common/global.h"
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"

class CTexture :
	public CAsset
{
private:
	ScratchImage m_Image; // 이미지 파일 로딩 및 저장 기능
	ComPtr<ID3D11Texture2D> m_Tex2D; // (ScratchImage)SysMem -> GPUMem
	ComPtr<ID3D11RenderTargetView> m_RTV;
	ComPtr<ID3D11DepthStencilView> m_DSV;
	ComPtr<ID3D11ShaderResourceView> m_SRV;
	ComPtr<ID3D11UnorderedAccessView> m_UAV;
	D3D11_TEXTURE2D_DESC m_Desc;
	int m_RecentSRVNum;
	int m_RecentUAVNum;
	bool m_bSystemMemoryReleased; // 시스템 메모리 해제 여부를 추적

public:
	UINT GetWidth() { return m_Desc.Width; }
	UINT GetHeight() { return m_Desc.Height; }

	tPixel* GetPixels()
	{
		if (m_bSystemMemoryReleased)
		{
			// 시스템 메모리가 해제된 경우 GPU에서 다시 캡처
			CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
			m_bSystemMemoryReleased = false;
		}
		return reinterpret_cast<tPixel*>(m_Image.GetPixels());
	}

	ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView> GetRTV() { return m_RTV; }
	ComPtr<ID3D11DepthStencilView> GetDSV() { return m_DSV; }
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_SRV; }
	const D3D11_TEXTURE2D_DESC& GetDesc() { return m_Desc; }

	const TexMetadata& GetMetaData()
	{
		if (m_bSystemMemoryReleased)
		{
			CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
			m_bSystemMemoryReleased = false;
		}
		return m_Image.GetMetadata();
	}

	size_t GetRowPitch()
	{
		if (m_bSystemMemoryReleased)
		{
			CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
			m_bSystemMemoryReleased = false;
		}
		return m_Image.GetImages()->rowPitch;
	}

	size_t GetSlicePitch()
	{
		if (m_bSystemMemoryReleased)
		{
			CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
			m_bSystemMemoryReleased = false;
		}
		return m_Image.GetImages()->slicePitch;
	}

	UINT GetArraySize() { return m_Desc.ArraySize; }
	bool IsCubeMap() { return (m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE); }

	void Binding(int _RegisterNum);
	static void Clear(int _RegisterNum);
	void Binding_SRV_CS(int _RegisterNum);
	void Binding_UAV_CS(int _RegisterNum);
	void Clear_SRV_CS();
	void Clear_UAV_CS();

	template<typename T>
	bool CaptureTextureCustom(vector<T>& _pixels);

	void ReleaseSystemMemory()
	{
		if (!m_bSystemMemoryReleased)
		{
			m_Image = ScratchImage();
			m_bSystemMemoryReleased = true;
		}
	}

private:
	int Load(const wstring& _FilePath) override;
	int Save(const wstring& _RelativePath) override;
	int Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _BindFlag,
			   D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
	int Create(ComPtr<ID3D11Texture2D> _Tex2D);
	int CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex);

public:
	int GenerateMip(UINT _Level);

	CLONE_DISABLE(CTexture);
	CTexture();
	~CTexture() override;

	friend class CAssetMgr;
};

template<typename T>
bool CTexture::CaptureTextureCustom(vector<T>& _pixels)
{
	ScratchImage capturedImage;
	if (FAILED(CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), capturedImage)))
		return false;

	// ScratchImage에서 직접 픽셀 데이터를 가져옵니다
	_pixels.assign((T*)capturedImage.GetPixels(), (T*)(capturedImage.GetPixels() + capturedImage.GetPixelsSize()));

	return true;
}

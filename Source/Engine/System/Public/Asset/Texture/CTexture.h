#pragma once
#include "Common/global.h"
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"

/**
 * @brief 표면에 입힐 이미지 데이터를 담당하는 클래스
 *
 * @param m_Image 이미지 파일 로딩 및 저장 기능
 * @param m_Tex2D (ScratchImage)SysMem -> GPUMem
 * @param m_RTV Render Target View
 * @param m_DSV Depth Stencil View
 * @param m_SRV Shader Resource View
 * @param m_UAV Unordered Access View
 * @param m_Desc
 * @param m_RecentSRVNum
 * @param m_RecentUAVNum
 * @param m_IsMemoryReleased 시스템 메모리 관리를 위한 변수
 */
class CTexture :
	public CAsset
{
	friend class CAssetMgr;

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
	bool m_IsMemoryReleased;

private:
	int Load(const wstring& PFilePath) override;
	int Save(const wstring& PRelativePath) override;
	int Create(UINT PWidth, UINT PHeight, DXGI_FORMAT PPixelFormat, UINT PBindFlag,
	           D3D11_USAGE PUsage = D3D11_USAGE_DEFAULT);
	int Create(ComPtr<ID3D11Texture2D> P2DTexture);
	int CreateArrayTexture(const vector<Ptr<CTexture>>& PTextureVector);

public:
	const TexMetadata& GetMetaData();
	tPixel* GetPixels();
	size_t GetRowPitch();
	size_t GetSlicePitch();
	void ReleaseSystemMemory();

	void Binding(int PRegisterNum);
	static void Clear(int PRegisterNum);
	static void Clear(int InStartSlot, int InEndSlot);
	void Binding_SRV_CS(int PRegisterNum);
	void Binding_UAV_CS(int PRegisterNum);
	void Clear_SRV_CS();
	void Clear_UAV_CS();
	int GenerateMip(UINT PLevel);

	CLONE_DISABLE(CTexture);
	CTexture();
	~CTexture() override;

	UINT GetWidth() const { return m_Desc.Width; }
	UINT GetHeight() const { return m_Desc.Height; }
	UINT GetArraySize() const { return m_Desc.ArraySize; }
	ComPtr<ID3D11Texture2D> GetTex2D() const { return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView> GetRTV() const { return m_RTV; }
	ComPtr<ID3D11DepthStencilView> GetDSV() const { return m_DSV; }
	ComPtr<ID3D11ShaderResourceView> GetSRV() const { return m_SRV; }
	const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_Desc; }
	bool IsCubeMap() const { return m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE; }

	template <typename T>
	bool CaptureTextureCustom(vector<T>& _pixels)
	{
		ScratchImage capturedImage;
		if (FAILED(CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), capturedImage)))
			return false;

		// ScratchImage에서 직접 픽셀 데이터를 가져옵니다
		_pixels.assign(reinterpret_cast<T*>(capturedImage.GetPixels()),
		               reinterpret_cast<T*>(capturedImage.GetPixels() + capturedImage.GetPixelsSize()));

		return true;
	}
};

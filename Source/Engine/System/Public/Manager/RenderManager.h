#pragma once
#include "Common/singleton.h"

#include "Engine/System/Public/Rendering/RenderTarget/MultiRenderTarget.h"

class CCamera;
class FGeometryRenderPass;
class FLightRenderPass;
class FMergeRenderPass;

class FRenderManager :
	public singleton<FRenderManager>
{
	SINGLE(FRenderManager)

private:
	FMultiRenderTarget* MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::END)];

	// SwapChain Texture
	Ptr<CTexture> RenderTargetTexture;
	Ptr<CTexture> DepthStencilTexture;

	// Geometry Buffer Texture
	Ptr<CTexture> ColorTargetTexture;
	Ptr<CTexture> NormalTargetTexture;
	Ptr<CTexture> WorldPositionTargetTexture;
	Ptr<CTexture> EmissiveTargetTexture;
	Ptr<CTexture> DataTargetTexture;

	// Light Texture
	Ptr<CTexture> DiffuseTargetTexture;
	Ptr<CTexture> SpecularTargetTexture;

	// Minimap Texture
	Ptr<CTexture> MinimapTargetTexture;
	Ptr<CTexture> MinimapDepthStencilTexture;

	// PostProcess Texture
	Ptr<CTexture> PostProcessTexture;

	// Render Materials
	Ptr<CMaterial> MergeMaterial;
	Ptr<CMaterial> DirectionalLightMaterial;
	Ptr<CMaterial> PointLightMaterial;

	// Debug Shape Materials
	Ptr<CMaterial> DebugShapeMaterial;
	Ptr<CMaterial> DebugShapeSphereMaterial;
	Ptr<CMaterial> DebugShapeLineMaterial;
	Ptr<CMaterial> DebugSkeletonMaterial;

	// Debug Dummy Object
	CGameObject* DebugObject;

	// Render Pass
	FGeometryRenderPass* GeometryPass;
	FLightRenderPass* LightPass;
	FMergeRenderPass* MergePass;

	// Status
	bool bIsEditorMode;
	bool bIsDebugRender;

	// Camera
	CCamera* EditorCamera;
	vector<CCamera*> CameraVector;

	// Light Info
	vector<tLight2DInfo> Light2DInfo;
	vector<tLight3DInfo> Light3DInfo;
	constexpr UINT MAX_LIGHT_COUNT = 256;

private:
	// Init
	void CreateResources();
	void CreateMultiRenderTargets();
	void CreateMaterials();
	void CreateRenderPasses();

	// Main
	void RenderStart();
	void RenderEditor();
	void RenderPlay();
	void RenderDebug();
	void RenderClear();

public:
	void Init();
	void Render();
	CCamera* GetMainCamera() const;
};

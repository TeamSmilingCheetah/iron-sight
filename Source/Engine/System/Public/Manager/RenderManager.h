#pragma once
#include "Common/singleton.h"

#include "Engine/System/Public/Rendering/RenderTarget/MultiRenderTarget.h"

class CLight3D;
class CLight2D;
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
	bool bShowFPS;

	// Camera
	CCamera* EditorCamera;
	vector<CCamera*> CameraVector;

	// Light Info
	vector<CLight2D*> Light2DVector;
	vector<CLight3D*> Light3DVector;
	vector<tLight2DInfo> Light2DInfo;
	vector<tLight3DInfo> Light3DInfo;

	CStructuredBuffer* Light2DBuffer;
	CStructuredBuffer* Light3DBuffer;
	CStructuredBuffer* LightInstancingBuffer;

	// Debug Render Task
	vector<tDebugShapeInfo> DebugInfoList;

	// Parameter Struct
	FRenderPassParameters PassParams;

	// Draw Call Unit Rendering
	vector<FRenderCommand> RenderCommands;

	Ptr<CTexture> SpecifyTarget;

private:
	// Init
	void CreateResources();
	void CreateMultiRenderTargets();
	void CreateMaterials();
	void CreateRenderPasses();

	// Main
	void RenderStart() const;
	void RenderEditor();
	void RenderPlay();
	void RenderDebug();
	static void RenderFPS();
	static void RenderClear();

	// RenderStart
	void ClassifyRenderObjects(CCamera* InCamera);
	void ExecuteRenderPipeline(CCamera* InCamera);
	static void UpdateViewProjection(CCamera* InCamera);
	void ClearMRT() const;

public:
	void Init();
	void Render();
	void CopyRenderTarget() const;
	void SetupFrameResources();
	void AddDebugShape(const tDebugShapeInfo& InDebugShapeInfo);

	void RegisterCamera(CCamera* InCamera, UINT InIdx);
	void UnregisterCamera(CCamera* InCamera);
	void SetEditorCamera(CCamera* InEditorCamera) { EditorCamera = InEditorCamera; }
	void UnregisterCameraAll() { CameraVector.clear(); }

	// Getter & Setter
	CCamera* GetMainCamera() const;
	int GetLight3DLastIndex() const { return static_cast<int>(Light3DVector.size()) - 1; }
	FMultiRenderTarget* GetRenderTarget(MRT_TYPE InType) const
	{
		return MultiRenderTargetArray[static_cast<UINT>(InType)];
	}

	void RegisterLight2D(CLight2D* InLight2D) { Light2DVector.push_back(InLight2D); }
	void RegisterLight3D(CLight3D* InLight3D) { Light3DVector.push_back(InLight3D); }

	bool IsEditorMode() const { return bIsEditorMode; }
	bool IsDebugRender() const { return bIsDebugRender; }

	void SetEditorMode(bool InIsEditorMode) { bIsEditorMode = InIsEditorMode; }
	bool SetDebugRender(bool InIsDebugRender) { return bIsDebugRender = InIsDebugRender; }
	void SetSpecifyTarget(Ptr<CTexture> InTarget) { SpecifyTarget = InTarget; }
};

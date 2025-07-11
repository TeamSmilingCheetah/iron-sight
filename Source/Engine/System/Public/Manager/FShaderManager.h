#pragma once
#include "FShaderInfoMap.h"

struct ShaderInfo;

// 1. ShaderID enum 정의 (예시)
enum class ShaderID : uint16_t {
    DECAL_VS,
    DECAL_PS,
    SKYBOX_VS,
    SKYBOX_PS,
    STD2D_VS,
    STD2D_PS,
    STD2D_ALPHA_PS,
    STD3D_VS,
    STD3D_PS,
    STD3D_DEFERRED_VS,
    STD3D_DEFERRED_PS,
    STD2D_PAPER_BURN_VS,
    STD2D_PAPER_BURN_PS,
    UI_VS,
    UI_PS,
    UI_CARDINAL_VS,
    UI_CARDINAL_PS,
    UI_HP_VS,
    UI_HP_PS,
    UI_ITEM_USE_VS,
    UI_ITEM_USE_PS,
    UI_CROSSHAIR_VS,
    UI_CROSSHAIR_PS,
    TILEMAP_VS,
    TILEMAP_PS,
    POSTPROCESS_VS,
    POSTPROCESS_PS,
    TARGETPOST_VS,
    TARGETPOST_PS,
    POSTPROCESS_DISTORTION_VS,
    POSTPROCESS_DISTORTION_PS,
    POSTPROCESS_VORTEX_VS,
    POSTPROCESS_VORTEX_PS,
    POSTPROCESS_BLUR_VS,
    POSTPROCESS_BLUR_PS,
    POSTPROCESS_EFFECT_MERGE_VS,
    POSTPROCESS_EFFECT_MERGE_PS,
    TESS_VS,
    TESS_HS,
    TESS_DS,
    TESS_PS,
    LANDSCAPE_VS,
    LANDSCAPE_HS,
    LANDSCAPE_DS,
    LANDSCAPE_PS,
    DEBUG_SHAPE_VS,
    DEBUG_SHAPE_PS,
    DEBUG_SHAPE_SPHERE_VS,
    DEBUG_SHAPE_SPHERE_PS,
    DEBUG_SHAPE_LINE_VS,
    DEBUG_SHAPE_LINE_PS,
    DEBUG_SHAPE_LINE_GS,
    DEBUG_SKELETON_VS,
    DEBUG_SKELETON_GS,
    DEBUG_SKELETON_PS,
    MERGE_VS,
    MERGE_PS,
    DIRECTIONAL_LIGHT_VS,
    DIRECTIONAL_LIGHT_PS,
    POINT_LIGHT_VS,
    POINT_LIGHT_PS,
    PARTICLE_VS,
    PARTICLE_GS,
    PARTICLE_PS,
    PARTICLE_TICK_CS,
    STD2D_EFFECT_VS,
    STD2D_EFFECT_PS,
    COPYBONE_CS,
    HEIGHTMAP_CS,
    RAYCAST_CS,
    WEIGHTMAP_CS,
    MESH_COLLISION_CS,
    COUNT
};

class FShaderManager
	: public singleton<FShaderManager>
{
	SINGLE(FShaderManager);

private:
    std::vector<ComPtr<ID3DBlob>> MShaderBlobVec;
	FShaderInfoMap* MShaderInfoMap;
	ComPtr<ID3DBlob> MErrBlob;

public:
	void Init();
	void RegisterShader(ShaderID id, ComPtr<ID3DBlob> PShaderBlob);
	// void BuildAllRegisteredShaders();
	ComPtr<ID3DBlob> GetBlob(ShaderID id);
	ComPtr<ID3DBlob> GetCompiledBlob(const wstring& PBlobFilePath, const wstring& PEffectsFilePath,
	                                 const wstring& PEntryPointName, const wstring& PShaderInfo);

private:
	void BuildAllShaders(const wstring& PFolderPath);
	void BuildShader(const wstring& PFullCSOPath, const ShaderInfo& PShaderInfo);
};

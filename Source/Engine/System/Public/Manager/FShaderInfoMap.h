#pragma once

#include "FShaderManager.h" // ShaderID enum 사용

struct FShaderInfoMap
{
    // ShaderID와 1:1로 매핑되는 vector
    std::vector<ShaderInfo> Data = {
        {L"decal_vs.cso", L"decal.fx", L"VS_Decal"}, // DECAL_VS
        {L"decal_ps.cso", L"decal.fx", L"PS_Decal"}, // DECAL_PS
        {L"skybox_vs.cso", L"skybox.fx", L"VS_SkyBox"}, // SKYBOX_VS
        {L"skybox_ps.cso", L"skybox.fx", L"PS_SkyBox"}, // SKYBOX_PS
        {L"std2d_vs.cso", L"std2d.fx", L"VS_Std2D"}, // STD2D_VS
        {L"std2d_ps.cso", L"std2d.fx", L"PS_Std2D"}, // STD2D_PS
        {L"std2d_ps_alpha.cso", L"std2d.fx", L"PS_Std2D_AlphaBlend"}, // STD2D_ALPHA_PS
        {L"std3d_vs.cso", L"std3d.fx", L"VS_Std3D"}, // STD3D_VS
        {L"std3d_ps.cso", L"std3d.fx", L"PS_Std3D"}, // STD3D_PS
        {L"std3d_deferred_vs.cso", L"std3d_deferred.fx", L"VS_Std3D_Deferred"}, // STD3D_DEFERRED_VS
        {L"std3d_deferred_ps.cso", L"std3d_deferred.fx", L"PS_Std3D_Deferred"}, // STD3D_DEFERRED_PS
        {L"std2d_paper_burn_vs.cso", L"std2d.fx", L"VS_Std2D"}, // STD2D_PAPER_BURN_VS
        {L"std2d_paper_burn_ps.cso", L"std2d.fx", L"PS_Std2D_PaperBurn"}, // STD2D_PAPER_BURN_PS
        {L"ui_vs.cso", L"ui.fx", L"VS_UI"}, // UI_VS
        {L"ui_ps.cso", L"ui.fx", L"PS_UI"}, // UI_PS
        {L"ui_cardinal_vs.cso", L"ui.fx", L"VS_UI_Cardinal"}, // UI_CARDINAL_VS
        {L"ui_cardinal_ps.cso", L"ui.fx", L"PS_UI"}, // UI_CARDINAL_PS
        {L"ui_hp_vs.cso", L"ui.fx", L"VS_UI"}, // UI_HP_VS
        {L"ui_hp_ps.cso", L"ui.fx", L"PS_UI_HP"}, // UI_HP_PS
        {L"ui_item_use_vs.cso", L"ui.fx", L"VS_UI"}, // UI_ITEM_USE_VS
        {L"ui_item_use_ps.cso", L"ui.fx", L"PS_UI_ItemUse"}, // UI_ITEM_USE_PS
        {L"ui_crosshair_vs.cso", L"ui.fx", L"VS_UI"}, // UI_CROSSHAIR_VS
        {L"ui_crosshair_ps.cso", L"ui.fx", L"PS_UI_Crosshair"}, // UI_CROSSHAIR_PS
        {L"tilemap_vs.cso", L"tilemap.fx", L"VS_TileMap"}, // TILEMAP_VS
        {L"tilemap_ps.cso", L"tilemap.fx", L"PS_TileMap"}, // TILEMAP_PS
        {L"postprocess_vs.cso", L"postprocess.fx", L"VS_Post"}, // POSTPROCESS_VS
        {L"postprocess_ps.cso", L"postprocess.fx", L"PS_Post"}, // POSTPROCESS_PS
        {L"targetpost_vs.cso", L"targetpost.fx", L"VS_TargetPost"}, // TARGETPOST_VS
        {L"targetpost_ps.cso", L"targetpost.fx", L"PS_TargetPost"}, // TARGETPOST_PS
        {L"postprocess_distortion_vs.cso", L"postprocess.fx", L"VS_Distortion"}, // POSTPROCESS_DISTORTION_VS
        {L"postprocess_distortion_ps.cso", L"postprocess.fx", L"PS_Distortion"}, // POSTPROCESS_DISTORTION_PS
        {L"postprocess_vortex_vs.cso", L"postprocess.fx", L"VS_Vortex"}, // POSTPROCESS_VORTEX_VS
        {L"postprocess_vortex_ps.cso", L"postprocess.fx", L"PS_Vortex"}, // POSTPROCESS_VORTEX_PS
        {L"postprocess_blur_vs.cso", L"postprocess.fx", L"VS_Blur"}, // POSTPROCESS_BLUR_VS
        {L"postprocess_blur_ps.cso", L"postprocess.fx", L"PS_Blur"}, // POSTPROCESS_BLUR_PS
        {L"postprocess_effect_merge_vs.cso", L"postprocess.fx", L"VS_EffectMerge"}, // POSTPROCESS_EFFECT_MERGE_VS
        {L"postprocess_effect_merge_ps.cso", L"postprocess.fx", L"PS_EffectMerge"}, // POSTPROCESS_EFFECT_MERGE_PS
        {L"tess_vs.cso", L"tess.fx", L"VS_Tess"}, // TESS_VS
        {L"tess_hs.cso", L"tess.fx", L"HS_Tess"}, // TESS_HS
        {L"tess_ds.cso", L"tess.fx", L"DS_Tess"}, // TESS_DS
        {L"tess_ps.cso", L"tess.fx", L"PS_Tess"}, // TESS_PS
        {L"landscape_vs.cso", L"landscape.fx", L"VS_LandScape"}, // LANDSCAPE_VS
        {L"landscape_hs.cso", L"landscape.fx", L"HS_LandScape"}, // LANDSCAPE_HS
        {L"landscape_ds.cso", L"landscape.fx", L"DS_LandScape"}, // LANDSCAPE_DS
        {L"landscape_ps.cso", L"landscape.fx", L"PS_LandScape"}, // LANDSCAPE_PS
        {L"debug_shape_vs.cso", L"debug.fx", L"VS_DebugShape"}, // DEBUG_SHAPE_VS
        {L"debug_shape_ps.cso", L"debug.fx", L"PS_DebugShape"}, // DEBUG_SHAPE_PS
        {L"debug_shape_sphere_vs.cso", L"debug.fx", L"VS_DebugShapeSphere"}, // DEBUG_SHAPE_SPHERE_VS
        {L"debug_shape_sphere_ps.cso", L"debug.fx", L"PS_DebugShapeSphere"}, // DEBUG_SHAPE_SPHERE_PS
        {L"debug_shape_line_vs.cso", L"debug.fx", L"VS_DebugShapeLine"}, // DEBUG_SHAPE_LINE_VS
        {L"debug_shape_line_ps.cso", L"debug.fx", L"PS_DebugShapeLine"}, // DEBUG_SHAPE_LINE_PS
        {L"debug_shape_line_gs.cso", L"debug.fx", L"GS_DebugShapeLine"}, // DEBUG_SHAPE_LINE_GS
        {L"debug_skeleton_vs.cso", L"debug.fx", L"VS_DebugSkeleton"}, // DEBUG_SKELETON_VS
        {L"debug_skeleton_gs.cso", L"debug.fx", L"GS_DebugSkeleton"}, // DEBUG_SKELETON_GS
        {L"debug_skeleton_ps.cso", L"debug.fx", L"PS_DebugSkeleton"}, // DEBUG_SKELETON_PS
        {L"merge_vs.cso", L"merge.fx", L"VS_Merge"}, // MERGE_VS
        {L"merge_ps.cso", L"merge.fx", L"PS_Merge"}, // MERGE_PS
        {L"light_dir_vs.cso", L"light.fx", L"VS_DirLight"}, // DIRECTIONAL_LIGHT_VS
        {L"light_dir_ps.cso", L"light.fx", L"PS_DirLight"}, // DIRECTIONAL_LIGHT_PS
        {L"light_point_vs.cso", L"light.fx", L"VS_PointLight"}, // POINT_LIGHT_VS
        {L"light_point_ps.cso", L"light.fx", L"PS_PointLight"}, // POINT_LIGHT_PS
        {L"particle_vs.cso", L"particle.fx", L"VS_Particle"}, // PARTICLE_VS
        {L"particle_gs.cso", L"particle.fx", L"GS_Particle"}, // PARTICLE_GS
        {L"particle_ps.cso", L"particle.fx", L"PS_Particle"}, // PARTICLE_PS
        {L"particle_tick_cs.cso", L"particle_tick.fx", L"CS_ParticleTick"}, // PARTICLE_TICK_CS
        {L"std2d_effect_vs.cso", L"std2d.fx", L"VS_Effect"}, // STD2D_EFFECT_VS
        {L"std2d_effect_ps.cso", L"std2d.fx", L"PS_Effect"}, // STD2D_EFFECT_PS
        {L"copybone_cs.cso", L"copybone.fx", L"CS_CopyBoneMatrix"}, // COPYBONE_CS
        {L"heightmap_cs.cso", L"heightmap.fx", L"CS_HeightMap"}, // HEIGHTMAP_CS
        {L"raycast_cs.cso", L"raycast.fx", L"CS_Raycast"}, // RAYCAST_CS
        {L"weightmap_cs.cso", L"weightmap.fx", L"CS_WeightMap"}, // WEIGHTMAP_CS
        {L"mesh_collision_cs.cso", L"mesh_collision.fx", L"CS_MeshCollision"} // MESH_COLLISION_CS
    };
};

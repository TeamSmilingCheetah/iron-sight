set(PROJECT_NAME Engine)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "API/Private/EngineFacade.cpp"
    "API/Public/EngineFacade.h"
    "Runtime/Private/Component/Physics/CMeshCollider.cpp"
    "Runtime/Public/Component/Physics/CMeshCollider.h"
    "System/Private/Manager/FCollisionBroad.cpp"
    "System/Private/Manager/FCollisionBVH.cpp"
    "System/Private/Manager/FCollisionManager.cpp"
    "System/Private/Manager/FCollisionNarrow.cpp"
    "System/Private/Manager/FCollisionRaycast.cpp"
    "System/Private/Manager/FLogManager.cpp"
    "System/Private/Rendering/Shader/CMeshCollisionCS.cpp"
    "System/Public/Manager/FCollisionManager.h"
    "System/Public/Manager/FLogManager.h"
    "System/Public/Rendering/Shader/CMeshCollisionCS.h"
)
source_group("" FILES ${no_group_source_files})

set(00._Default
    "pch.cpp"
    "pch.h"
)
source_group("00. Default" FILES ${00._Default})

set(01._Header
    "${CMAKE_SOURCE_DIR}/External/Include/Common/define.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/enum.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/extern.cpp"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/func.cpp"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/func.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/global.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/Ptr.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/SimpleMath.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/SimpleMath.inl"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/singleton.h"
    "${CMAKE_SOURCE_DIR}/External/Include/Common/struct.h"
    "Runtime/Public/Component/Base/components.h"
    "System/Public/Asset/Base/assets.h"
)
source_group("01. Header" FILES ${01._Header})

set(02._Engine
    "Core/Private/CEngine.cpp"
    "Core/Public/CEngine.h"
)
source_group("02. Engine" FILES ${02._Engine})

set(03._Device
    "System/Private/Rendering/Device/CDevice.cpp"
    "System/Public/Rendering/Device/CDevice.h"
)
source_group("03. Device" FILES ${03._Device})

set(03._Device__ConstBuffer
    "System/Private/Rendering/Buffer/CConstBuffer.cpp"
    "System/Public/Rendering/Buffer/CConstBuffer.h"
)
source_group("03. Device\\ConstBuffer" FILES ${03._Device__ConstBuffer})

set(03._Device__StructuredBuffer
    "System/Private/Rendering/Buffer/CStructuredBuffer.cpp"
    "System/Public/Rendering/Buffer/CStructuredBuffer.h"
)
source_group("03. Device\\StructuredBuffer" FILES ${03._Device__StructuredBuffer})

set(04._Manager__01._Path
    "System/Private/Manager/CPathMgr.cpp"
    "System/Public/Manager/CPathMgr.h"
)
source_group("04. Manager\\01. Path" FILES ${04._Manager__01._Path})

set(04._Manager__02._Time
    "System/Private/Manager/CTimeMgr.cpp"
    "System/Public/Manager/CTimeMgr.h"
)
source_group("04. Manager\\02. Time" FILES ${04._Manager__02._Time})

set(04._Manager__03._Key
    "System/Private/Manager/CKeyMgr.cpp"
    "System/Public/Manager/CKeyMgr.h"
)
source_group("04. Manager\\03. Key" FILES ${04._Manager__03._Key})

set(04._Manager__04._Asset
    "System/Private/Manager/CAssetMgr.cpp"
    "System/Private/Manager/CAssetMgr_Init.cpp"
    "System/Private/Manager/CAssetMgr_Mesh.cpp"
    "System/Public/Manager/CAssetMgr.h"
)
source_group("04. Manager\\04. Asset" FILES ${04._Manager__04._Asset})

set(04._Manager__05._Level
    "System/Private/Manager/CLevelMgr.cpp"
    "System/Public/Manager/CLevelMgr.h"
)
source_group("04. Manager\\05. Level" FILES ${04._Manager__05._Level})

set(04._Manager__06._Render
    "System/Private/Manager/CRenderMgr.cpp"
    "System/Private/Manager/CRenderMgr_Init.cpp"
    "System/Public/Manager/CRenderMgr.h"
)
source_group("04. Manager\\06. Render" FILES ${04._Manager__06._Render})

set(04._Manager__06._Render__MRT
    "System/Private/Rendering/RenderTarget/CMRT.cpp"
    "System/Public/Rendering/RenderTarget/CMRT.h"
)
source_group("04. Manager\\06. Render\\MRT" FILES ${04._Manager__06._Render__MRT})

set(04._Manager__07._TaskMgr
    "System/Private/Manager/CTaskMgr.cpp"
    "System/Public/Manager/CTaskMgr.h"
)
source_group("04. Manager\\07. TaskMgr" FILES ${04._Manager__07._TaskMgr})

set(04._Manager__08._FontMgr
    "System/Private/Manager/CFontMgr.cpp"
    "System/Public/Manager/CFontMgr.h"
)
source_group("04. Manager\\08. FontMgr" FILES ${04._Manager__08._FontMgr})

set(04._Manager__09._InstancingBuffer
    "System/Private/Rendering/Buffer/CInstancingBuffer.cpp"
    "System/Public/Rendering/Buffer/CInstancingBuffer.h"
)
source_group("04. Manager\\09. InstancingBuffer" FILES ${04._Manager__09._InstancingBuffer})

set(04._Manager__10._SoundMgr
    "System/Private/Manager/CSoundMgr.cpp"
    "System/Public/Manager/CSoundMgr.h"
)
source_group("04. Manager\\10. SoundMgr" FILES ${04._Manager__10._SoundMgr})

set(04._Manager__10._UIMgr
    "System/Private/Manager/CUIMgr.cpp"
    "System/Public/Manager/CUIMgr.h"
)
source_group("04. Manager\\10. UIMgr" FILES ${04._Manager__10._UIMgr})

set(04._Manager__12._ObjectPollMgr
    "System/Private/Manager/CObjectPoolMgr.cpp"
    "System/Public/Manager/CObjectPoolMgr.h"
)
source_group("04. Manager\\12. ObjectPollMgr" FILES ${04._Manager__12._ObjectPollMgr})

set(05._Level
    "Runtime/Private/Actor/CLayer.cpp"
    "Runtime/Private/Actor/CLevel.cpp"
    "Runtime/Public/Actor/CLayer.h"
    "Runtime/Public/Actor/CLevel.h"
)
source_group("05. Level" FILES ${05._Level})

set(06._GameObject
    "Runtime/Private/Actor/CGameObject.cpp"
    "Runtime/Public/Actor/CGameObject.h"
)
source_group("06. GameObject" FILES ${06._GameObject})

set(07._Component
    "Runtime/Private/Component/Base/CComponent.cpp"
    "Runtime/Public/Component/Base/CComponent.h"
)
source_group("07. Component" FILES ${07._Component})

set(07._Component__01._Transform
    "Runtime/Private/Component/Transform/CTransform.cpp"
    "Runtime/Public/Component/Transform/CTransform.h"
)
source_group("07. Component\\01. Transform" FILES ${07._Component__01._Transform})

set(07._Component__02._Camera
    "Runtime/Private/Component/Camera/CCamera.cpp"
    "Runtime/Public/Component/Camera/CCamera.h"
)
source_group("07. Component\\02. Camera" FILES ${07._Component__02._Camera})

set(07._Component__02._Camera__Frustum
    "Runtime/Private/Component/Camera/CFrustum.cpp"
    "Runtime/Public/Component/Camera/CFrustum.h"
)
source_group("07. Component\\02. Camera\\Frustum" FILES ${07._Component__02._Camera__Frustum})

set(07._Component__04._Collider3D
    "Runtime/Private/Component/Physics/CCollider2D.cpp"
    "Runtime/Private/Component/Physics/CCollider3D.cpp"
    "Runtime/Private/Component/Physics/CColliderRay.cpp"
    "Runtime/Public/Component/Physics/CCollider2D.h"
    "Runtime/Public/Component/Physics/CCollider3D.h"
    "Runtime/Public/Component/Physics/CColliderRay.h"
)
source_group("07. Component\\04. Collider3D" FILES ${07._Component__04._Collider3D})

set(07._Component__05._FlipbookPlayer
    "Runtime/Private/Component/Animation/CFlipbookPlayer.cpp"
    "Runtime/Public/Component/Animation/CFlipbookPlayer.h"
)
source_group("07. Component\\05. FlipbookPlayer" FILES ${07._Component__05._FlipbookPlayer})

set(07._Component__06._Light2D
    "Runtime/Private/Component/Light/CLight2D.cpp"
    "Runtime/Public/Component/Light/CLight2D.h"
)
source_group("07. Component\\06. Light2D" FILES ${07._Component__06._Light2D})

set(07._Component__07._Light3D
    "Runtime/Private/Component/Light/CLight3D.cpp"
    "Runtime/Public/Component/Light/CLight3D.h"
)
source_group("07. Component\\07. Light3D" FILES ${07._Component__07._Light3D})

set(07._Component__08._Animator3D
    "Runtime/Private/Component/Animation/CAnimator3D.cpp"
    "Runtime/Public/Component/Animation/CAnimator3D.h"
)
source_group("07. Component\\08. Animator3D" FILES ${07._Component__08._Animator3D})

set(07._Component__09._UI
    "Runtime/Private/Component/UI/CUI.cpp"
    "Runtime/Public/Component/UI/CUI.h"
)
source_group("07. Component\\09. UI" FILES ${07._Component__09._UI})

set(07._Component__RenderComponent
    "Runtime/Private/Component/Base/CRenderComponent.cpp"
    "Runtime/Public/Component/Base/CRenderComponent.h"
)
source_group("07. Component\\RenderComponent" FILES ${07._Component__RenderComponent})

set(07._Component__RenderComponent__01._MeshRender
    "Runtime/Private/Component/Rendering/CMeshRender.cpp"
    "Runtime/Public/Component/Rendering/CMeshRender.h"
)
source_group("07. Component\\RenderComponent\\01. MeshRender" FILES ${07._Component__RenderComponent__01._MeshRender})

set(07._Component__RenderComponent__02._TileMap
    "Runtime/Private/Component/Rendering/CTileMap.cpp"
    "Runtime/Public/Component/Rendering/CTileMap.h"
)
source_group("07. Component\\RenderComponent\\02. TileMap" FILES ${07._Component__RenderComponent__02._TileMap})

set(07._Component__RenderComponent__03._ParticleSystem
    "Runtime/Private/Component/Rendering/CParticleSystem.cpp"
    "Runtime/Public/Component/Rendering/CParticleSystem.h"
)
source_group("07. Component\\RenderComponent\\03. ParticleSystem" FILES ${07._Component__RenderComponent__03._ParticleSystem})

set(07._Component__RenderComponent__04._SkyBox
    "Runtime/Private/Component/Rendering/CSkyBox.cpp"
    "Runtime/Public/Component/Rendering/CSkyBox.h"
)
source_group("07. Component\\RenderComponent\\04. SkyBox" FILES ${07._Component__RenderComponent__04._SkyBox})

set(07._Component__RenderComponent__05._Decal
    "Runtime/Private/Component/Rendering/CDecal.cpp"
    "Runtime/Public/Component/Rendering/CDecal.h"
)
source_group("07. Component\\RenderComponent\\05. Decal" FILES ${07._Component__RenderComponent__05._Decal})

set(07._Component__RenderComponent__06._LandScape
    "Runtime/Private/Component/Rendering/CLandScape.cpp"
    "Runtime/Public/Component/Rendering/CLandScape.h"
    "System/Private/Manager/CLandScape_Init.cpp"
)
source_group("07. Component\\RenderComponent\\06. LandScape" FILES ${07._Component__RenderComponent__06._LandScape})

set(07._Component__RenderComponent__07._UIRender
    "Runtime/Private/Component/Rendering/CUIRender.cpp"
    "Runtime/Public/Component/Rendering/CUIRender.h"
)
source_group("07. Component\\RenderComponent\\07. UIRender" FILES ${07._Component__RenderComponent__07._UIRender})

set(07._Component__Script
    "Runtime/Private/Component/Script/CScript.cpp"
    "Runtime/Public/Component/Script/CScript.h"
)
source_group("07. Component\\Script" FILES ${07._Component__Script})

set(08._Asset
    "System/Private/Asset/Base/CAsset.cpp"
    "System/Public/Asset/Base/CAsset.h"
)
source_group("08. Asset" FILES ${08._Asset})

set(08._Asset__01._Mesh
    "System/Private/Asset/Mesh/CMesh.cpp"
    "System/Public/Asset/Mesh/CMesh.h"
)
source_group("08. Asset\\01. Mesh" FILES ${08._Asset__01._Mesh})

set(08._Asset__02._MeshData
    "System/Private/Asset/Mesh/CMeshData.cpp"
    "System/Public/Asset/Mesh/CMeshData.h"
)
source_group("08. Asset\\02. MeshData" FILES ${08._Asset__02._MeshData})

set(08._Asset__03._Texture
    "System/Private/Asset/Texture/CTexture.cpp"
    "System/Public/Asset/Texture/CTexture.h"
)
source_group("08. Asset\\03. Texture" FILES ${08._Asset__03._Texture})

set(08._Asset__04._Sound
    "System/Private/Asset/Audio/CSound.cpp"
    "System/Public/Asset/Audio/CSound.h"
)
source_group("08. Asset\\04. Sound" FILES ${08._Asset__04._Sound})

set(08._Asset__05._Prefab
    "System/Private/Asset/Prefab/CPrefab.cpp"
    "System/Public/Asset/Prefab/CPrefab.h"
)
source_group("08. Asset\\05. Prefab" FILES ${08._Asset__05._Prefab})

set(08._Asset__06._FlipBook
    "System/Private/Asset/Texture/CFlipbook.cpp"
    "System/Public/Asset/Texture/CFlipbook.h"
)
source_group("08. Asset\\06. FlipBook" FILES ${08._Asset__06._FlipBook})

set(08._Asset__07._Sprite
    "System/Private/Asset/Texture/CSprite.cpp"
    "System/Public/Asset/Texture/CSprite.h"
)
source_group("08. Asset\\07. Sprite" FILES ${08._Asset__07._Sprite})

set(08._Asset__08._Animation
    "System/Private/Asset/Animation/CAnimation.cpp"
    "System/Public/Asset/Animation/CAnimation.h"
)
source_group("08. Asset\\08. Animation" FILES ${08._Asset__08._Animation})

set(08._Asset__09._Skeleton
    "System/Private/Asset/Animation/CSkeleton.cpp"
    "System/Public/Asset/Animation/CSkeleton.h"
)
source_group("08. Asset\\09. Skeleton" FILES ${08._Asset__09._Skeleton})

set(08._Asset__10._Material
    "System/Private/Rendering/Material/CMaterial.cpp"
    "System/Public/Rendering/Material/CMaterial.h"
)
source_group("08. Asset\\10. Material" FILES ${08._Asset__10._Material})

set(08._Asset__Shader
    "System/Private/Rendering/Shader/CShader.cpp"
    "System/Public/Rendering/Shader/CShader.h"
)
source_group("08. Asset\\Shader" FILES ${08._Asset__Shader})

set(08._Asset__Shader__ComputeShader
    "System/Private/Rendering/Shader/CComputeShader.cpp"
    "System/Public/Rendering/Shader/CComputeShader.h"
)
source_group("08. Asset\\Shader\\ComputeShader" FILES ${08._Asset__Shader__ComputeShader})

set(08._Asset__Shader__GraphicShader
    "System/Private/Rendering/Shader/CGraphicShader.cpp"
    "System/Public/Rendering/Shader/CGraphicShader.h"
)
source_group("08. Asset\\Shader\\GraphicShader" FILES ${08._Asset__Shader__GraphicShader})

set(HLSL__03._Compute__3DAnimation__BoneMatric
    "System/Private/Rendering/Shader/CBoneMatrixCS.cpp"
    "System/Public/Rendering/Shader/CBoneMatrixCS.h"
)
source_group("HLSL\\03. Compute\\3DAnimation\\BoneMatric" FILES ${HLSL__03._Compute__3DAnimation__BoneMatric})

set(HLSL__03._Compute__3DAnimation__CopyBone
    "System/Private/Rendering/Shader/CCopyBoneCS.cpp"
    "System/Public/Rendering/Shader/CCopyBoneCS.h"
)
source_group("HLSL\\03. Compute\\3DAnimation\\CopyBone" FILES ${HLSL__03._Compute__3DAnimation__CopyBone})

set(HLSL__03._Compute__ClearColor
    "System/Private/Rendering/Shader/CClearColorCS.cpp"
    "System/Public/Rendering/Shader/CClearColorCS.h"
)
source_group("HLSL\\03. Compute\\ClearColor" FILES ${HLSL__03._Compute__ClearColor})

set(HLSL__03._Compute__LandScape__HeightMap
    "System/Private/Rendering/Shader/CHeightMapCS.cpp"
    "System/Public/Rendering/Shader/CHeightMapCS.h"
)
source_group("HLSL\\03. Compute\\LandScape\\HeightMap" FILES ${HLSL__03._Compute__LandScape__HeightMap})

set(HLSL__03._Compute__LandScape__RayCast
    "System/Private/Rendering/Shader/CRaycastCS.cpp"
    "System/Public/Rendering/Shader/CRaycastCS.h"
)
source_group("HLSL\\03. Compute\\LandScape\\RayCast" FILES ${HLSL__03._Compute__LandScape__RayCast})

set(HLSL__03._Compute__LandScape__WeightMap
    "System/Private/Rendering/Shader/CWeightMapCS.cpp"
    "System/Public/Rendering/Shader/CWeightMapCS.h"
)
source_group("HLSL\\03. Compute\\LandScape\\WeightMap" FILES ${HLSL__03._Compute__LandScape__WeightMap})

set(HLSL__03._Compute__ParticleTick
    "System/Private/Rendering/Shader/CParticleTickCS.cpp"
    "System/Public/Rendering/Shader/CParticleTickCS.h"
)
source_group("HLSL\\03. Compute\\ParticleTick" FILES ${HLSL__03._Compute__ParticleTick})

set(Module
    "Core/Private/CEntity.cpp"
    "Core/Public/CEntity.h"
    "System/Private/Rendering/Tool/FBX/CFBXLoader.cpp"
    "System/Public/Rendering/Tool/FBX/CFBXLoader.h"
)
source_group("Module" FILES ${Module})

set(ALL_FILES
    ${no_group_source_files}
    ${00._Default}
    ${01._Header}
    ${02._Engine}
    ${03._Device}
    ${03._Device__ConstBuffer}
    ${03._Device__StructuredBuffer}
    ${04._Manager__01._Path}
    ${04._Manager__02._Time}
    ${04._Manager__03._Key}
    ${04._Manager__04._Asset}
    ${04._Manager__05._Level}
    ${04._Manager__06._Render}
    ${04._Manager__06._Render__MRT}
    ${04._Manager__07._TaskMgr}
    ${04._Manager__08._FontMgr}
    ${04._Manager__09._InstancingBuffer}
    ${04._Manager__10._SoundMgr}
    ${04._Manager__10._UIMgr}
    ${04._Manager__12._ObjectPollMgr}
    ${05._Level}
    ${06._GameObject}
    ${07._Component}
    ${07._Component__01._Transform}
    ${07._Component__02._Camera}
    ${07._Component__02._Camera__Frustum}
    ${07._Component__04._Collider3D}
    ${07._Component__05._FlipbookPlayer}
    ${07._Component__06._Light2D}
    ${07._Component__07._Light3D}
    ${07._Component__08._Animator3D}
    ${07._Component__09._UI}
    ${07._Component__RenderComponent}
    ${07._Component__RenderComponent__01._MeshRender}
    ${07._Component__RenderComponent__02._TileMap}
    ${07._Component__RenderComponent__03._ParticleSystem}
    ${07._Component__RenderComponent__04._SkyBox}
    ${07._Component__RenderComponent__05._Decal}
    ${07._Component__RenderComponent__06._LandScape}
    ${07._Component__RenderComponent__07._UIRender}
    ${07._Component__Script}
    ${08._Asset}
    ${08._Asset__01._Mesh}
    ${08._Asset__02._MeshData}
    ${08._Asset__03._Texture}
    ${08._Asset__04._Sound}
    ${08._Asset__05._Prefab}
    ${08._Asset__06._FlipBook}
    ${08._Asset__07._Sprite}
    ${08._Asset__08._Animation}
    ${08._Asset__09._Skeleton}
    ${08._Asset__10._Material}
    ${08._Asset__Shader}
    ${08._Asset__Shader__ComputeShader}
    ${08._Asset__Shader__GraphicShader}
    ${HLSL__03._Compute__3DAnimation__BoneMatric}
    ${HLSL__03._Compute__3DAnimation__CopyBone}
    ${HLSL__03._Compute__ClearColor}
    ${HLSL__03._Compute__LandScape__HeightMap}
    ${HLSL__03._Compute__LandScape__RayCast}
    ${HLSL__03._Compute__LandScape__WeightMap}
    ${HLSL__03._Compute__ParticleTick}
    ${Module}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_include_directories(Engine PUBLIC
        ${CMAKE_SOURCE_DIR}/External/Include
        ${CMAKE_SOURCE_DIR}/External/Include/DirectXMath
        ${CMAKE_SOURCE_DIR}/External/Include/FBXLoader
        ${CMAKE_SOURCE_DIR}/Source/
        ${CMAKE_SOURCE_DIR}/Source/Engine
)

target_precompile_headers(Engine PRIVATE "pch.h")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Engine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_d"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/External/Library/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/External/Library/${PROJECT_NAME}/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_LIB;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_LIB;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    endif()
endif()

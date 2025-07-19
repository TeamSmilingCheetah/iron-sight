set(PROJECT_NAME Client)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "API/Private/ClientFacade.cpp"
    "API/Public/ClientFacade.h"
    "Core/Private/FClientApp.cpp"
    "Core/Private/FWindow.cpp"
    "Core/Public/FClientApp.h"
    "Core/Public/FWindow.h"
    "main.cpp"
    "UI/Private/Editor/AddObjectUI.cpp"
    "UI/Private/PrefabEditor/PrefabEditor.cpp"
    "UI/Public/Editor/AddObjectUI.h"
    "UI/Public/PrefabEditor/PrefabEditor.h"
)
source_group("" FILES ${no_group_source_files})

set(00._Default
    "Build/framework.h"
    "Build/Resource.h"
    "Build/targetver.h"
    "pch.cpp"
    "pch.h"
)
source_group("00. Default" FILES ${00._Default})

set(01._ImGui__1._ImGui
    "imgui/imconfig.h"
    "imgui/imgui.cpp"
    "imgui/imgui.h"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_impl_dx11.cpp"
    "imgui/imgui_impl_dx11.h"
    "imgui/imgui_impl_win32.cpp"
    "imgui/imgui_impl_win32.h"
    "imgui/imgui_internal.h"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
)
source_group("01. ImGui\\1. ImGui" FILES ${01._ImGui__1._ImGui})

set(01._ImGui__2._ImGuiMgr
    "System/Private/CImGuiMgr.cpp"
    "System/Public/CImGuiMgr.h"
)
source_group("01. ImGui\\2. ImGuiMgr" FILES ${01._ImGui__2._ImGuiMgr})

set(02._EditorMgr
    "Core/Private/CEditorMgr.cpp"
    "Core/Public/CEditorMgr.h"
)
source_group("02. EditorMgr" FILES ${02._EditorMgr})

set(02._EditorMgr__EditorCamScript
    "Script/Private/CEditorCamScript.cpp"
    "Script/Private/CEditorSpaceCamScript.cpp"
    "Script/Public/CEditorCamScript.h"
    "Script/Public/CEditorSpaceCamScript.h"
)
source_group("02. EditorMgr\\EditorCamScript" FILES ${02._EditorMgr__EditorCamScript})

set(02._EditorMgr__GameObjectEx
    "Script/Private/CGameObjectEx.cpp"
    "Script/Public/CGameObjectEx.h"
)
source_group("02. EditorMgr\\GameObjectEx" FILES ${02._EditorMgr__GameObjectEx})

set(03._EditorUI
    "UI/Private/Editor/EditorUI.cpp"
    "UI/Public/Editor/EditorUI.h"
)
source_group("03. EditorUI" FILES ${03._EditorUI})

set(03._EditorUI__ContentUI
    "UI/Private/Editor/ContentUI.cpp"
    "UI/Public/Editor/ContentUI.h"
)
source_group("03. EditorUI\\ContentUI" FILES ${03._EditorUI__ContentUI})

set(03._EditorUI__Inspector
    "UI/Private/Editor/Inspector.cpp"
    "UI/Public/Editor/Inspector.h"
)
source_group("03. EditorUI\\Inspector" FILES ${03._EditorUI__Inspector})

set(03._EditorUI__Inspector__AssetUI
    "UI/Private/Asset/AssetUI.cpp"
    "UI/Public/Asset/AssetUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI" FILES ${03._EditorUI__Inspector__AssetUI})

set(03._EditorUI__Inspector__AssetUI__1._MeshUI
    "UI/Private/Asset/MeshUI.cpp"
    "UI/Public/Asset/MeshUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\1. MeshUI" FILES ${03._EditorUI__Inspector__AssetUI__1._MeshUI})

set(03._EditorUI__Inspector__AssetUI__10._ComputeShaderUI
    "Rendering/Private/ComputeShaderUI.cpp"
    "Rendering/Public/ComputeShaderUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\10. ComputeShaderUI" FILES ${03._EditorUI__Inspector__AssetUI__10._ComputeShaderUI})

set(03._EditorUI__Inspector__AssetUI__11._AnimationUI
    "UI/Private/Asset/AnimationUI.cpp"
    "UI/Public/Asset/AnimationUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\11. AnimationUI" FILES ${03._EditorUI__Inspector__AssetUI__11._AnimationUI})

set(03._EditorUI__Inspector__AssetUI__12._SkeletonUI
    "UI/Private/Asset/SkeletonUI.cpp"
    "UI/Public/Asset/SkeletonUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\12. SkeletonUI" FILES ${03._EditorUI__Inspector__AssetUI__12._SkeletonUI})

set(03._EditorUI__Inspector__AssetUI__2._MeshDataUI
    "UI/Private/Asset/MeshDataUI.cpp"
    "UI/Public/Asset/MeshDataUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\2. MeshDataUI" FILES ${03._EditorUI__Inspector__AssetUI__2._MeshDataUI})

set(03._EditorUI__Inspector__AssetUI__3._TextureUI
    "UI/Private/Asset/TextureUI.cpp"
    "UI/Public/Asset/TextureUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\3. TextureUI" FILES ${03._EditorUI__Inspector__AssetUI__3._TextureUI})

set(03._EditorUI__Inspector__AssetUI__4._SoundUI
    "UI/Private/Asset/SoundUI.cpp"
    "UI/Public/Asset/SoundUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\4. SoundUI" FILES ${03._EditorUI__Inspector__AssetUI__4._SoundUI})

set(03._EditorUI__Inspector__AssetUI__5._PrefabUI
    "UI/Private/Asset/PrefabUI.cpp"
    "UI/Public/Asset/PrefabUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\5. PrefabUI" FILES ${03._EditorUI__Inspector__AssetUI__5._PrefabUI})

set(03._EditorUI__Inspector__AssetUI__6._FlipbookUI
    "UI/Private/Asset/FlipbookUI.cpp"
    "UI/Public/Asset/FlipbookUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\6. FlipbookUI" FILES ${03._EditorUI__Inspector__AssetUI__6._FlipbookUI})

set(03._EditorUI__Inspector__AssetUI__7._SpriteUI
    "UI/Private/Asset/SpriteUI.cpp"
    "UI/Public/Asset/SpriteUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\7. SpriteUI" FILES ${03._EditorUI__Inspector__AssetUI__7._SpriteUI})

set(03._EditorUI__Inspector__AssetUI__8._MaterialUI
    "UI/Private/Asset/MaterialUI.cpp"
    "UI/Public/Asset/MaterialUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\8. MaterialUI" FILES ${03._EditorUI__Inspector__AssetUI__8._MaterialUI})

set(03._EditorUI__Inspector__AssetUI__9._GraphicShaderUI
    "Rendering/Private/GraphicShaderUI.cpp"
    "Rendering/Public/GraphicShaderUI.h"
)
source_group("03. EditorUI\\Inspector\\AssetUI\\9. GraphicShaderUI" FILES ${03._EditorUI__Inspector__AssetUI__9._GraphicShaderUI})

set(03._EditorUI__Inspector__ComponentUI
    "UI/Private/Component/ComponentUI.cpp"
    "UI/Public/Component/ComponentUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI" FILES ${03._EditorUI__Inspector__ComponentUI})

set(03._EditorUI__Inspector__ComponentUI__Animator3DUI
    "UI/Private/Component/Animator3DUI.cpp"
    "UI/Public/Component/Animator3DUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Animator3DUI" FILES ${03._EditorUI__Inspector__ComponentUI__Animator3DUI})

set(03._EditorUI__Inspector__ComponentUI__CameraUI
    "UI/Private/Component/CameraUI.cpp"
    "UI/Public/Component/CameraUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\CameraUI" FILES ${03._EditorUI__Inspector__ComponentUI__CameraUI})

set(03._EditorUI__Inspector__ComponentUI__Collider2DUI
    "UI/Private/Component/Collider2DUI.cpp"
    "UI/Public/Component/Collider2DUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Collider2DUI" FILES ${03._EditorUI__Inspector__ComponentUI__Collider2DUI})

set(03._EditorUI__Inspector__ComponentUI__Collider3DUI
    "UI/Private/Component/Collider3DUI.cpp"
    "UI/Public/Component/Collider3DUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Collider3DUI" FILES ${03._EditorUI__Inspector__ComponentUI__Collider3DUI})

set(03._EditorUI__Inspector__ComponentUI__ColliderRayUI
    "UI/Private/Component/ColliderRayUI.cpp"
    "UI/Public/Component/ColliderRayUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\ColliderRayUI" FILES ${03._EditorUI__Inspector__ComponentUI__ColliderRayUI})

set(03._EditorUI__Inspector__ComponentUI__Decal
    "UI/Private/Component/DecalUI.cpp"
    "UI/Public/Component/DecalUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Decal" FILES ${03._EditorUI__Inspector__ComponentUI__Decal})

set(03._EditorUI__Inspector__ComponentUI__LandscapeUI
    "UI/Private/Component/LandScapeUI.cpp"
    "UI/Public/Component/LandScapeUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\LandscapeUI" FILES ${03._EditorUI__Inspector__ComponentUI__LandscapeUI})

set(03._EditorUI__Inspector__ComponentUI__Light2D
    "UI/Private/Component/Light2DUI.cpp"
    "UI/Public/Component/Light2DUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Light2D" FILES ${03._EditorUI__Inspector__ComponentUI__Light2D})

set(03._EditorUI__Inspector__ComponentUI__Light3D
    "UI/Private/Component/Light3DUI.cpp"
    "UI/Public/Component/Light3DUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\Light3D" FILES ${03._EditorUI__Inspector__ComponentUI__Light3D})

set(03._EditorUI__Inspector__ComponentUI__MeshColliderUI
    "UI/Private/Component/MeshColliderUI.cpp"
    "UI/Public/Component/MeshColliderUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\MeshColliderUI" FILES ${03._EditorUI__Inspector__ComponentUI__MeshColliderUI})

set(03._EditorUI__Inspector__ComponentUI__MeshRenderUI
    "UI/Private/Component/MeshRenderUI.cpp"
    "UI/Public/Component/MeshRenderUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\MeshRenderUI" FILES ${03._EditorUI__Inspector__ComponentUI__MeshRenderUI})

set(03._EditorUI__Inspector__ComponentUI__ParticleUI
    "UI/Private/Component/ParticleUI.cpp"
    "UI/Public/Component/ParticleUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\ParticleUI" FILES ${03._EditorUI__Inspector__ComponentUI__ParticleUI})

set(03._EditorUI__Inspector__ComponentUI__ScriptUI
    "UI/Private/Component/ScriptUI.cpp"
    "UI/Public/Component/ScriptUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\ScriptUI" FILES ${03._EditorUI__Inspector__ComponentUI__ScriptUI})

set(03._EditorUI__Inspector__ComponentUI__SkyBoxUI
    "UI/Private/Component/SkyBoxUI.cpp"
    "UI/Public/Component/SkyBoxUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\SkyBoxUI" FILES ${03._EditorUI__Inspector__ComponentUI__SkyBoxUI})

set(03._EditorUI__Inspector__ComponentUI__TransformUI
    "UI/Private/Component/TransformUI.cpp"
    "UI/Public/Component/TransformUI.h"
)
source_group("03. EditorUI\\Inspector\\ComponentUI\\TransformUI" FILES ${03._EditorUI__Inspector__ComponentUI__TransformUI})

set(03._EditorUI__List
    "UI/Private/Editor/ListUI.cpp"
    "UI/Public/Editor/ListUI.h"
)
source_group("03. EditorUI\\List" FILES ${03._EditorUI__List})

set(03._EditorUI__MenuUI
    "UI/Private/Editor/MenuUI.cpp"
    "UI/Public/Editor/MenuUI.h"
)
source_group("03. EditorUI\\MenuUI" FILES ${03._EditorUI__MenuUI})

set(03._EditorUI__Outliner
    "UI/Private/Editor/Outliner.cpp"
    "UI/Public/Editor/Outliner.h"
)
source_group("03. EditorUI\\Outliner" FILES ${03._EditorUI__Outliner})

set(03._EditorUI__ParamUI
    "UI/Private/Editor/ParamUI.cpp"
    "UI/Public/Editor/ParamUI.h"
)
source_group("03. EditorUI\\ParamUI" FILES ${03._EditorUI__ParamUI})

set(03._EditorUI__SpriteEditor
    "UI/Private/SpriteEditor/SpriteEditor.cpp"
    "UI/Public/SpriteEditor/SpriteEditor.h"
)
source_group("03. EditorUI\\SpriteEditor" FILES ${03._EditorUI__SpriteEditor})

set(03._EditorUI__SpriteEditor__Sub
    "UI/Private/SpriteEditor/SE_Sub.cpp"
    "UI/Public/SpriteEditor/SE_Sub.h"
)
source_group("03. EditorUI\\SpriteEditor\\Sub" FILES ${03._EditorUI__SpriteEditor__Sub})

set(03._EditorUI__SpriteEditor__Sub__SE_AtlasView
    "UI/Private/SpriteEditor/SE_AtlasView.cpp"
    "UI/Public/SpriteEditor/SE_AtlasView.h"
)
source_group("03. EditorUI\\SpriteEditor\\Sub\\SE_AtlasView" FILES ${03._EditorUI__SpriteEditor__Sub__SE_AtlasView})

set(03._EditorUI__SpriteEditor__Sub__SE_Detail
    "UI/Private/SpriteEditor/SE_Detail.cpp"
    "UI/Public/SpriteEditor/SE_Detail.h"
)
source_group("03. EditorUI\\SpriteEditor\\Sub\\SE_Detail" FILES ${03._EditorUI__SpriteEditor__Sub__SE_Detail})

set(03._EditorUI__TargetOBUI
    "UI/Private/TargetOBUI.cpp"
    "UI/Public/TargetOBUI.h"
)
source_group("03. EditorUI\\TargetOBUI" FILES ${03._EditorUI__TargetOBUI})

set(03._EditorUI__TreeUI
    "UI/Private/Editor/TreeUI.cpp"
    "UI/Public/Editor/TreeUI.h"
)
source_group("03. EditorUI\\TreeUI" FILES ${03._EditorUI__TreeUI})

set(ALL_FILES
    ${no_group_source_files}
    ${00._Default}
    ${01._ImGui__1._ImGui}
    ${01._ImGui__2._ImGuiMgr}
    ${02._EditorMgr}
    ${02._EditorMgr__EditorCamScript}
    ${02._EditorMgr__GameObjectEx}
    ${03._EditorUI}
    ${03._EditorUI__ContentUI}
    ${03._EditorUI__Inspector}
    ${03._EditorUI__Inspector__AssetUI}
    ${03._EditorUI__Inspector__AssetUI__1._MeshUI}
    ${03._EditorUI__Inspector__AssetUI__10._ComputeShaderUI}
    ${03._EditorUI__Inspector__AssetUI__11._AnimationUI}
    ${03._EditorUI__Inspector__AssetUI__12._SkeletonUI}
    ${03._EditorUI__Inspector__AssetUI__2._MeshDataUI}
    ${03._EditorUI__Inspector__AssetUI__3._TextureUI}
    ${03._EditorUI__Inspector__AssetUI__4._SoundUI}
    ${03._EditorUI__Inspector__AssetUI__5._PrefabUI}
    ${03._EditorUI__Inspector__AssetUI__6._FlipbookUI}
    ${03._EditorUI__Inspector__AssetUI__7._SpriteUI}
    ${03._EditorUI__Inspector__AssetUI__8._MaterialUI}
    ${03._EditorUI__Inspector__AssetUI__9._GraphicShaderUI}
    ${03._EditorUI__Inspector__ComponentUI}
    ${03._EditorUI__Inspector__ComponentUI__Animator3DUI}
    ${03._EditorUI__Inspector__ComponentUI__CameraUI}
    ${03._EditorUI__Inspector__ComponentUI__Collider2DUI}
    ${03._EditorUI__Inspector__ComponentUI__Collider3DUI}
    ${03._EditorUI__Inspector__ComponentUI__ColliderRayUI}
    ${03._EditorUI__Inspector__ComponentUI__Decal}
    ${03._EditorUI__Inspector__ComponentUI__LandscapeUI}
    ${03._EditorUI__Inspector__ComponentUI__Light2D}
    ${03._EditorUI__Inspector__ComponentUI__Light3D}
    ${03._EditorUI__Inspector__ComponentUI__MeshColliderUI}
    ${03._EditorUI__Inspector__ComponentUI__MeshRenderUI}
    ${03._EditorUI__Inspector__ComponentUI__ParticleUI}
    ${03._EditorUI__Inspector__ComponentUI__ScriptUI}
    ${03._EditorUI__Inspector__ComponentUI__SkyBoxUI}
    ${03._EditorUI__Inspector__ComponentUI__TransformUI}
    ${03._EditorUI__List}
    ${03._EditorUI__MenuUI}
    ${03._EditorUI__Outliner}
    ${03._EditorUI__ParamUI}
    ${03._EditorUI__SpriteEditor}
    ${03._EditorUI__SpriteEditor__Sub}
    ${03._EditorUI__SpriteEditor__Sub__SE_AtlasView}
    ${03._EditorUI__SpriteEditor__Sub__SE_Detail}
    ${03._EditorUI__TargetOBUI}
    ${03._EditorUI__TreeUI}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} WIN32 ${ALL_FILES})

target_precompile_headers(Client PRIVATE "pch.h")

target_include_directories(Client PUBLIC
        ${CMAKE_SOURCE_DIR}/External/Include
        ${CMAKE_SOURCE_DIR}/External/Include/DirectXMath
        ${CMAKE_SOURCE_DIR}/External/Include/FBXLoader
        ${CMAKE_SOURCE_DIR}/External/Include/DirectxTex
        ${CMAKE_SOURCE_DIR}/External/Include/FMOD
        ${CMAKE_SOURCE_DIR}/Source/
        ${CMAKE_SOURCE_DIR}/Source/Client
)

# 외부 라이브러리 경로 추가
link_directories(
    ${CMAKE_SOURCE_DIR}/External/Library/Engine
    ${CMAKE_SOURCE_DIR}/External/Library/Game
    ${CMAKE_SOURCE_DIR}/External/Library/DirectXTex
    ${CMAKE_SOURCE_DIR}/External/Library/FBXLoader/x64/debug
    ${CMAKE_SOURCE_DIR}/External/Library/FMOD
)

target_link_libraries(Client PRIVATE
    Engine
    Game
    #${CMAKE_SOURCE_DIR}/External/Library/Engine/libEngine_d.a
    #${CMAKE_SOURCE_DIR}/External/Library/Game/libGame_d.a
    ${CMAKE_SOURCE_DIR}/External/Library/FBXLoader/x64/debug/libfbxsdk-md.lib
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/External/Library/DirectXTex/DirectXTex_debug.lib>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/External/Library/DirectXTex/DirectXTex.lib>
    ${CMAKE_SOURCE_DIR}/External/Library/FMOD/fmod64_vc.lib
    ${CMAKE_SOURCE_DIR}/External/Library/FontEngine/FW1FontWrapper.lib

    # DirectX 및 Windows 기본 라이브러리
    d3d11
    d3dcompiler
    dxgi
    dwrite
    dwmapi
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
    shell32
    user32
    gdi32
    winspool
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Client)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/OutputFile/Bin/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/OutputFile/Bin/"
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
        "_WINDOWS;"
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
        "_WINDOWS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Engine
    Game
)


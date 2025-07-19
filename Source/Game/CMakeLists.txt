set(PROJECT_NAME Game)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Factory/Private/GameFactory.cpp"
    "Factory/Public/GameFactory.h"
    "Gameplay/Character/Private/EnemyVisionScript.cpp"
)
source_group("" FILES ${no_group_source_files})

set(00._Default
    "framework.h"
    "pch.cpp"
    "pch.h"
)
source_group("00. Default" FILES ${00._Default})

set(01._ScriptMgr
    "System/Private/GameplayManager.cpp"
    "System/Public/GameplayManager.h"
)
source_group("01. ScriptMgr" FILES ${01._ScriptMgr})

set(02._Gameplay
    "Gameplay/Character/Private/CameraController.cpp"
    "Gameplay/Character/Public/CameraController.h"
    "Gameplay/Particle/Private/ParticleController.cpp"
    "Gameplay/Particle/Public/ParticleController.h"
    "Gameplay/Projectile/Private/MissileProjectile.cpp"
    "Gameplay/Projectile/Public/MissileProjectile.h"
)
source_group("02. Gameplay" FILES ${02._Gameplay})

set(02._Gameplay__Door
    "Gameplay/Door/Private/DoorScript.cpp"
    "Gameplay/Door/Public/DoorScript.h"
)
source_group("02. Gameplay\\Door" FILES ${02._Gameplay__Door})

set(02._Gameplay__Enemy
    "Gameplay/Character/Private/EnemyController.cpp"
    "Gameplay/Character/Private/TestCharacter.cpp"
    "Gameplay/Character/Public/EnemyController.h"
    "Gameplay/Character/Public/EnemyVisionScript.h"
    "Gameplay/Character/Public/TestCharacter.h"
)
source_group("02. Gameplay\\Enemy" FILES ${02._Gameplay__Enemy})

set(02._Gameplay__Interaction
    "Gameplay/Character/Private/InteractionHandler.cpp"
    "Gameplay/Character/Public/InteractionHandler.h"
    "Gameplay/Interaction/Private/InteractableScript.cpp"
    "Gameplay/Interaction/Public/InteractableScript.h"
)
source_group("02. Gameplay\\Interaction" FILES ${02._Gameplay__Interaction})

set(02._Gameplay__Inventory
    "Gameplay/Inventory/Private/InventoryController.cpp"
    "Gameplay/Inventory/Private/Item.cpp"
    "Gameplay/Inventory/Private/ItemMgr.cpp"
    "Gameplay/Inventory/Private/UI_Inventory.cpp"
    "Gameplay/Inventory/Private/UI_Item.cpp"
    "Gameplay/Inventory/Private/UI_Vicinity.cpp"
    "Gameplay/Inventory/Public/InventoryController.h"
    "Gameplay/Inventory/Public/Item.h"
    "Gameplay/Inventory/Public/ItemMgr.h"
    "Gameplay/Inventory/Public/UI_Inventory.h"
    "Gameplay/Inventory/Public/UI_Item.h"
    "Gameplay/Inventory/Public/UI_Vicinity.h"
)
source_group("02. Gameplay\\Inventory" FILES ${02._Gameplay__Inventory})

set(02._Gameplay__Player
    "Gameplay/Character/Private/PlayerCharacter.cpp"
    "Gameplay/Character/Private/PlayerCharacter_Move.cpp"
    "Gameplay/Character/Public/PlayerCharacter.h"
)
source_group("02. Gameplay\\Player" FILES ${02._Gameplay__Player})

set(02._Gameplay__TestSound
    "Gameplay/TestSound.cpp"
    "Gameplay/TestSound.h"
)
source_group("02. Gameplay\\TestSound" FILES ${02._Gameplay__TestSound})

set(02._Gameplay__UI
    "Gameplay/UI/Private/KillinfoUIScript.cpp"
    "Gameplay/UI/Private/MinimapCameraScript.cpp"
    "Gameplay/UI/Private/MinimapUIScript.cpp"
    "Gameplay/UI/Private/RoundsUIScript.cpp"
    "Gameplay/UI/Public/KillinfoUIScript.h"
    "Gameplay/UI/Public/MinimapCameraScript.h"
    "Gameplay/UI/Public/MinimapUIScript.h"
    "Gameplay/UI/Public/RoundsUIScript.h"
)
source_group("02. Gameplay\\UI" FILES ${02._Gameplay__UI})

set(02._Gameplay__Weapon
    "Gameplay/Weapon/Private/BombController.cpp"
    "Gameplay/Weapon/Private/GunController.cpp"
    "Gameplay/Weapon/Private/ThrowableController.cpp"
    "Gameplay/Weapon/Private/WeaponController.cpp"
    "Gameplay/Weapon/Public/BombController.h"
    "Gameplay/Weapon/Public/GunController.h"
    "Gameplay/Weapon/Public/ThrowableController.h"
    "Gameplay/Weapon/Public/WeaponController.h"
)
source_group("02. Gameplay\\Weapon" FILES ${02._Gameplay__Weapon})

set(03._Level
    "Level/Private/TestLevel.cpp"
    "Level/Public/TestLevel.h"
)
source_group("03. Level" FILES ${03._Level})

set(ALL_FILES
    ${no_group_source_files}
    ${00._Default}
    ${01._ScriptMgr}
    ${02._Gameplay}
    ${02._Gameplay__Door}
    ${02._Gameplay__Enemy}
    ${02._Gameplay__Interaction}
    ${02._Gameplay__Inventory}
    ${02._Gameplay__Player}
    ${02._Gameplay__TestSound}
    ${02._Gameplay__UI}
    ${02._Gameplay__Weapon}
    ${03._Level}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_include_directories(Game PUBLIC
        ${CMAKE_SOURCE_DIR}/External/Include
        ${CMAKE_SOURCE_DIR}/External/Include/DirectXMath
        ${CMAKE_SOURCE_DIR}/External/Include/FBXLoader
        ${CMAKE_SOURCE_DIR}/Source/
        ${CMAKE_SOURCE_DIR}/Source/Game
)

target_precompile_headers(Game PRIVATE "pch.h")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "pch.h"
)

if (MSVC)
    add_compile_options(/EHsc)
endif()

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Game)

target_link_libraries(Client PRIVATE Engine)

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
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Engine
)


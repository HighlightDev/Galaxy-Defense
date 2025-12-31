set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/PHEngine/cmake)
include(Utility)

set(HEADERS
    src/Implementation/Components/MovementComponents/OnRouteMovementComponent.h
    src/Implementation/Components/UiComponents/SpaceObjectUiComponent.h
    src/Implementation/Components/ComponentData/SpaceObjectUiComponentData.h
    src/Implementation/Levels/GameLevelFactory.h
    src/Implementation/Levels/LevelBase.h
    src/Implementation/Levels/LevelSerializationHelper.h
    src/Implementation/Levels/LevelData.h
    src/Implementation/Levels/CombatLevel/CombatLevel.h
    src/Implementation/Levels/CombatLevel/Controllers/CombatLevelUiController.h
    src/Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h
    src/Implementation/Levels/CombatLevel/SmartPicker.h
    src/Implementation/Levels/MainMenu/MainMenuLevel.h
    src/Implementation/Levels/MainMenu/Controllers/MainMenuLevelUiController.h
    src/Implementation/Levels/Editor/EditorLevel.h
    src/Implementation/Levels/Editor/Controllers/LevelEditorUiController.h
    src/Implementation/Levels/Editor/Controllers/LevelEditorController.h
    src/Implementation/Levels/Editor/LevelPlacementGrid.h
    src/Implementation/Levels/Editor/RoutesHandler.h
    src/Implementation/Levels/Editor/TowersHandler.h
    src/Implementation/Levels/Editor/BarriersHandler.h
    src/Implementation/GalaxySceneCamera.h
    src/Implementation/GameObjectsType.h
    src/Implementation/GameObjectsCollisionType.h
    src/Implementation/StatusTypes.h
    src/Implementation/MissileType.h
    src/Implementation/DamageDealerType.h
    src/Implementation/ICameraTransformChangeNotifyable.h
    src/Implementation/Controllers/ILevelController.h
    src/Implementation/Controllers/CombatController.h
    src/Implementation/Controllers/AiActorController.h
    src/Implementation/Controllers/AiSpaceshipActorController.h
    src/Implementation/Controllers/UserInteractionController.h
    src/Implementation/Controllers/NavigationController.h
    src/Implementation/Controllers/BarriersController.h
    src/Implementation/Events/ElectroRaySphereContactCollisionEvent.h
    src/Implementation/Events/ShootRayCollisionEvent.h
    src/Implementation/Events/MainPlayerStatusChangedEvent.h
    src/Implementation/Events/ChangeGameModeEvent.h
    src/Implementation/Events/ChangeEditModeEvent.h
    src/Implementation/Events/LevelProgressChangedEvent.h
    src/Implementation/GameModeTypeEnum.h
    src/Implementation/EditModeTypeEnum.h
    src/Implementation/Factories/ISpaceShipFactory.h
    src/Implementation/Factories/IMissileFactory.h
    src/Implementation/Factories/ISpaceObjectFactory.h
    src/Implementation/Factories/WeakSpaceShipFactory.h
    src/Implementation/Factories/BombMissileFactory.h
    src/Implementation/Factories/FreezingMissileFactory.h
    src/Implementation/Factories/BlackHoleMissileFactory.h
    src/Implementation/Factories/AsteroidFactory.h
    src/Implementation/Factories/BackgroundPlanetsFactory.h
    src/Implementation/Factories/ElectroRayFactory.h
    src/Implementation/Factories/FreezingRayFactory.h
    src/Implementation/Factories/ElectroRayChainFactory.h
    src/Implementation/Factories/ISpaceStationFactory.h
    src/Implementation/Factories/SpaceStationFactory.h
    src/Implementation/Factories/BarrierFactory.h
    src/Implementation/Factories/SpawnPortalFactory.h
    src/Implementation/ActorLeveling/SpaceStationLevel.h
    src/Implementation/ActorLeveling/SpaceshipLevel.h
    src/Implementation/ActorLeveling/LevelAttributeDataProvider.h
    src/Implementation/Modifiers/IModifiable.h
    src/Implementation/Modifiers/GravityModifier.h
    src/Implementation/Modifiers/FreezingModifier.h
    src/Implementation/Modifiers/FreezingRayModifier.h
    src/Implementation/Modifiers/ElectroRayChainModifier.h
    src/Implementation/Modifiers/ModifiersHandler.h
    src/Implementation/Actors/ElectroRayChainActor.h
    src/Implementation/Actors/MissileActor.h
    src/Implementation/Actors/BombMissileActor.h
    src/Implementation/Actors/FreezingMissileActor.h
    src/Implementation/Actors/BlackHoleMissileActor.h
    src/Implementation/Actors/SpaceshipActor.h
    src/Implementation/Actors/WeakSpaceshipActor.h
    src/Implementation/Actors/SpaceObjectActor.h
    src/Implementation/Actors/AsteroidActor.h
    src/Implementation/Actors/ElectroRayActor.h
    src/Implementation/Actors/FreezingRayActor.h
    src/Implementation/Actors/SpaceStationActor.h
    src/Implementation/Actors/BarrierActor.h
    src/Implementation/Actors/BackgroundSpaceObjectActor.h
    src/Implementation/Actors/PortalActor.h
    src/Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h
    src/Implementation/MissileExplosionVisitors/BlackHoleExplosionVisitor.h
    src/Implementation/MissileExplosionVisitors/BombExplosionVisitor.h
    src/Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.h
    src/Implementation/MissileExplosionVisitors/ElectroRayExplosionVisitor.h
    src/Implementation/MissileExplosionVisitors/FreezingRayExplosionVisitor.h
    src/Implementation/LuaExecutors/LuaUiControllerExecutor.h
    src/Implementation/LuaExecutors/LuaCombatLevelExecutor.h
    src/Implementation/LuaExecutors/LuaCombatLevelUiControllerExecutor.h
    src/Implementation/LuaFunctions/LuaGameEventsFunctions.h
    src/Implementation/LuaFunctions/LuaLvlProgressFunctions.h
    src/Implementation/DataProviders/PlayerDataProvider.h
    src/Implementation/DataProviders/LevelDataProvider.h
    src/Implementation/DataProviders/GameConstants.h
    src/Implementation/LevelProgressSystem/LevelProgressStage.h
    src/Implementation/LevelProgressSystem/LevelRequirementTrackers/ILevelRequirementTracker.h
    src/Implementation/LevelProgressSystem/LevelRequirementTrackers/DestroySpaceshipsTracker.h
    src/Implementation/LevelProgressSystem/LevelRequirementTrackers/MissedSpaceshipsTracker.h
    src/Implementation/LevelProgressSystem/LevelRequirementTrackerJsonFactories/ILevelRequirementTrackerFactory.h
    src/Implementation/LevelProgressSystem/LevelRequirementTrackerJsonFactories/LevelRequirementsTrackerFactory.h
    src/Implementation/Navigation/NavigationPathBuilder.h
    src/Implementation/Navigation/PathSegment.h
    src/Implementation/Navigation/Path.h)

set(SOURCES
    src/Implementation/Components/MovementComponents/OnRouteMovementComponent.cpp
    src/Implementation/Components/UiComponents/SpaceObjectUiComponent.cpp
    src/Implementation/Levels/GameLevelFactory.cpp
    src/Implementation/Levels/LevelBase.cpp
    src/Implementation/Levels/LevelSerializationHelper.cpp
    src/Implementation/Levels/LevelData.cpp
    src/Implementation/Levels/CombatLevel/CombatLevel.cpp
    src/Implementation/Levels/CombatLevel/Controllers/CombatLevelUiController.cpp
    src/Implementation/Levels/CombatLevel/CombatActorsPoolHandler.cpp
    src/Implementation/Levels/CombatLevel/SmartPicker.cpp
    src/Implementation/Levels/MainMenu/MainMenuLevel.cpp
    src/Implementation/Levels/MainMenu/Controllers/MainMenuLevelUiController.cpp
    src/Implementation/Levels/Editor/EditorLevel.cpp
    src/Implementation/Levels/Editor/Controllers/LevelEditorUiController.cpp
    src/Implementation/Levels/Editor/Controllers/LevelEditorController.cpp
    src/Implementation/Levels/Editor/LevelPlacementGrid.cpp
    src/Implementation/Levels/Editor/RoutesHandler.cpp
    src/Implementation/Levels/Editor/TowersHandler.cpp
    src/Implementation/Levels/Editor/BarriersHandler.cpp
    src/Implementation/GalaxySceneCamera.cpp
    src/Implementation/Controllers/CombatController.cpp
    src/Implementation/Controllers/AiActorController.cpp
    src/Implementation/Controllers/AiSpaceshipActorController.cpp
    src/Implementation/Controllers/UserInteractionController.cpp
    src/Implementation/Controllers/NavigationController.cpp
    src/Implementation/Controllers/BarriersController.cpp
    src/Implementation/Events/ElectroRaySphereContactCollisionEvent.cpp
    src/Implementation/Events/ShootRayCollisionEvent.cpp
    src/Implementation/Events/MainPlayerStatusChangedEvent.cpp
    src/Implementation/Events/ChangeGameModeEvent.cpp
    src/Implementation/Events/ChangeEditModeEvent.cpp
    src/Implementation/Events/LevelProgressChangedEvent.cpp
    src/Implementation/Factories/WeakSpaceShipFactory.cpp
    src/Implementation/Factories/BombMissileFactory.cpp
    src/Implementation/Factories/FreezingMissileFactory.cpp
    src/Implementation/Factories/BlackHoleMissileFactory.cpp
    src/Implementation/Factories/AsteroidFactory.cpp
    src/Implementation/Factories/BackgroundPlanetsFactory.cpp
    src/Implementation/Factories/ElectroRayFactory.cpp
    src/Implementation/Factories/FreezingRayFactory.cpp
    src/Implementation/Factories/ElectroRayChainFactory.cpp
    src/Implementation/Factories/SpaceStationFactory.cpp
    src/Implementation/Factories/BarrierFactory.cpp
    src/Implementation/Factories/SpawnPortalFactory.cpp
    src/Implementation/ActorLeveling/SpaceStationLevel.cpp
    src/Implementation/ActorLeveling/SpaceshipLevel.cpp
    src/Implementation/ActorLeveling/LevelAttributeDataProvider.cpp
    src/Implementation/Modifiers/GravityModifier.cpp
    src/Implementation/Modifiers/FreezingModifier.cpp
    src/Implementation/Modifiers/FreezingRayModifier.cpp
    src/Implementation/Modifiers/ElectroRayChainModifier.cpp
    src/Implementation/Modifiers/ModifiersHandler.cpp
    src/Implementation/Actors/ElectroRayChainActor.cpp
    src/Implementation/Actors/MissileActor.cpp
    src/Implementation/Actors/BombMissileActor.cpp
    src/Implementation/Actors/FreezingMissileActor.cpp
    src/Implementation/Actors/BlackHoleMissileActor.cpp
    src/Implementation/Actors/SpaceshipActor.cpp
    src/Implementation/Actors/WeakSpaceshipActor.cpp
    src/Implementation/Actors/SpaceObjectActor.cpp
    src/Implementation/Actors/AsteroidActor.cpp
    src/Implementation/Actors/ElectroRayActor.cpp
    src/Implementation/Actors/FreezingRayActor.cpp
    src/Implementation/Actors/SpaceStationActor.cpp
    src/Implementation/Actors/BarrierActor.cpp
    src/Implementation/Actors/BackgroundSpaceObjectActor.cpp
    src/Implementation/Actors/PortalActor.cpp
    src/Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.cpp
    src/Implementation/MissileExplosionVisitors/BlackHoleExplosionVisitor.cpp
    src/Implementation/MissileExplosionVisitors/BombExplosionVisitor.cpp
    src/Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.cpp
    src/Implementation/MissileExplosionVisitors/ElectroRayExplosionVisitor.cpp
    src/Implementation/MissileExplosionVisitors/FreezingRayExplosionVisitor.cpp
    src/Implementation/LuaExecutors/LuaUiControllerExecutor.cpp
    src/Implementation/LuaExecutors/LuaCombatLevelExecutor.cpp
    src/Implementation/LuaExecutors/LuaCombatLevelUiControllerExecutor.cpp
    src/Implementation/LuaFunctions/LuaGameEventsFunctions.cpp
    src/Implementation/LuaFunctions/LuaLvlProgressFunctions.cpp
    src/Implementation/DataProviders/GameConstants.cpp
    src/Implementation/DataProviders/PlayerDataProvider.cpp
    src/Implementation/DataProviders/LevelDataProvider.cpp
    src/Implementation/LevelProgressSystem/LevelProgressStage.cpp
    src/Implementation/LevelProgressSystem/LevelProgressController.cpp
    src/Implementation/LevelProgressSystem/LevelRequirementTrackers/DestroySpaceshipsTracker.cpp
    src/Implementation/LevelProgressSystem/LevelRequirementTrackers/MissedSpaceshipsTracker.cpp
    src/Implementation/LevelProgressSystem/LevelRequirementTrackerJsonFactories/LevelRequirementsTrackerFactory.cpp
    src/Implementation/Navigation/NavigationPathBuilder.cpp
    src/Implementation/Navigation/PathSegment.cpp
    src/Implementation/Navigation/Path.cpp
    main.cpp)

add_compile_definitions(DEBUG GLM_ENABLE_EXPERIMENTAL)

set(EXECUTABLE_NAME GalaxyDefense)

add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})

add_custom_command(
  TARGET ${EXECUTABLE_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Start copy of resources to build directory: $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/PHEngine/res/
          $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(GAME_SRC_INCLUDE ${PROJECT_SOURCE_DIR}/${EXECUTABLE_NAME}/src/)
set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(LOGGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(UNWIND_TOOL_INCLUDE ${PROJECT_SOURCE_DIR}/UnwindTool/includes/)
set(GLEW_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLFW_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)
set(NLOHMANN_JSON_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/json/nlohmann/include/)
set(BULLET_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)
set(SNDFILE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/include/)
set(FREE_TYPE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/FreeType/include/)

target_include_directories(
  ${EXECUTABLE_NAME}
  PUBLIC ${GAME_SRC_INCLUDE}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
  PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
  PUBLIC ${LOGGER_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${BULLET_DEPENDENCE_INCLUDE}
  PUBLIC ${BULLET_PHYS_INCLUDE}
  PUBLIC ${SNDFILE_INCLUDE}
  PUBLIC ${NLOHMANN_JSON_INCLUDE}
  PUBLIC ${FREE_TYPE_INCLUDE})

set(BULLET_LIBS
    Bullet3Collision
    BulletCollision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Geometry
    BulletDynamics
    LinearMath
    BulletInverseDynamics
    ConvexDecomposition
    BulletRobotics
    BulletSoftBody)

if(WIN32)
  message("ARCH_x64: ${ARCH_x64}")
  if(ARCH_x64)
    set(LIB_LOCATION_PREFIX Win/x64/)
  else()
    set(LIB_LOCATION_PREFIX Win/x86/)
  endif(ARCH_x64)

  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    glew32
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/${LIB_LOCATION_PREFIX}/Release/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    lua54
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    assimp-vc143-mtd
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    sndfile
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC glfw3
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/${LIB_LOCATION_PREFIX}/
  )
  findlibrariesandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    BULLET_LIBS
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    PHEngine
    PUBLIC
    freetype
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/FreeType/lib/${LIB_LOCATION_PREFIX}/Release/
  )
elseif(UNIX)
  set(LIB_LOCATION_PREFIX "Unix")

  target_link_libraries(${EXECUTABLE_NAME} PRIVATE X11)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CMAKE_DL_LIBS})
  endif()

  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC assimp
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC lua
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC GLEW
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC sndfile
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/Unix/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC glfw3
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/Unix/)
  findlibrariesandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC BULLET_LIBS
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC freetype
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/FreeType/lib/Unix/Release/)
endif()

target_link_libraries(
  ${EXECUTABLE_NAME}
  PRIVATE ${OPENGL_LIBRARIES}
  PRIVATE TinyLogger
  PRIVATE PHEngine)

message(STATUS "LibUnwind enabled: ${ENABLE_LIBUNWIND}")
if(ENABLE_LIBUNWIND)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE UnwindTool)
else()

endif(ENABLE_LIBUNWIND)

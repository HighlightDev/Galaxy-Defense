set(
	HEADERS
	src/GameLevelFactory.h
	src/IntroLevel.h
	src/Implementation/SpaceSceneCamera.h
	src/Implementation/AiBrain.h
	src/Implementation/ICameraTransformChangeNotifyable.h
	src/Implementation/Controllers/ILevelController.h
	src/Implementation/Controllers/SpaceShipPlayerController.h
	src/Implementation/Controllers/CombatController.h
	src/Implementation/Controllers/UiController.h
	src/Implementation/Controllers/SceneController.h
	src/Implementation/Controllers/AiActorController.h
	src/Implementation/Controllers/AiSpaceshipActorController.h
	src/Implementation/Events/MainPlayerActionEvent.h
	src/Implementation/MainPlayerActionEnum.h
	src/Implementation/Factories/ISpaceShipFactory.h
	src/Implementation/Factories/IMissileFactory.h
	src/Implementation/Factories/ISpaceObjectFactory.h
	src/Implementation/Factories/WeakSpaceShipFactory.h
	src/Implementation/Factories/BombMissileFactory.h
	src/Implementation/Factories/FreezingMissileFactory.h
	src/Implementation/Factories/BlackHoleMissileFactory.h
	src/Implementation/Factories/AsteroidFactory.h
	src/Implementation/Factories/BackgroundPlanetsFactory.h
	src/Implementation/Modifiers/IModifiable.h
	src/Implementation/Modifiers/GravityModifier.h
	src/Implementation/Modifiers/FreezingModifier.h
	src/Implementation/Actors/MissileActor.h
	src/Implementation/Actors/BombMissileActor.h
	src/Implementation/Actors/FreezingMissileActor.h
	src/Implementation/Actors/BlackHoleMissileActor.h
	src/Implementation/Actors/SpaceshipActor.h
	src/Implementation/Actors/WeakSpaceshipActor.h
	src/Implementation/Actors/SpaceObjectActor.h
	src/Implementation/Actors/AsteroidActor.h
	src/Implementation/Actors/BackgroundSpaceObjectActor.h
	src/Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.h
	src/Implementation/MissileExplosionVisitors/BlackHoleExplosionVisitor.h
	src/Implementation/MissileExplosionVisitors/BombExplosionVisitor.h
	src/Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.h
	src/Implementation/Ui/PauseMenuUi.h
)

set(
	SOURCES
	src/GameLevelFactory.cpp
	src/IntroLevel.cpp
	src/Implementation/SpaceSceneCamera.cpp
	src/Implementation/Controllers/SpaceShipPlayerController.cpp
	src/Implementation/Controllers/CombatController.cpp
	src/Implementation/Controllers/UiController.cpp
	src/Implementation/AiBrain.cpp
	src/Implementation/Controllers/SceneController.cpp
	src/Implementation/Controllers/AiActorController.cpp
	src/Implementation/Controllers/AiSpaceshipActorController.cpp
	src/Implementation/Events/MainPlayerActionEvent.cpp
	src/Implementation/Factories/WeakSpaceShipFactory.cpp
	src/Implementation/Factories/BombMissileFactory.cpp
	src/Implementation/Factories/FreezingMissileFactory.cpp
	src/Implementation/Factories/BlackHoleMissileFactory.cpp
	src/Implementation/Factories/AsteroidFactory.cpp
	src/Implementation/Factories/BackgroundPlanetsFactory.cpp
	src/Implementation/Modifiers/GravityModifier.cpp
	src/Implementation/Modifiers/FreezingModifier.cpp
	src/Implementation/Actors/MissileActor.cpp
	src/Implementation/Actors/BombMissileActor.cpp
	src/Implementation/Actors/FreezingMissileActor.cpp
	src/Implementation/Actors/BlackHoleMissileActor.cpp
	src/Implementation/Actors/SpaceshipActor.cpp
	src/Implementation/Actors/WeakSpaceshipActor.cpp
	src/Implementation/Actors/SpaceObjectActor.cpp
	src/Implementation/Actors/AsteroidActor.cpp
	src/Implementation/Actors/BackgroundSpaceObjectActor.cpp
	src/Implementation/MissileExplosionVisitors/MissileExplosionVisitorBase.cpp
	src/Implementation/MissileExplosionVisitors/BlackHoleExplosionVisitor.cpp
	src/Implementation/MissileExplosionVisitors/BombExplosionVisitor.cpp
	src/Implementation/MissileExplosionVisitors/FreezingExplosionVisitor.cpp
	src/Implementation/Ui/PauseMenuUi.cpp
    main.cpp
)

add_compile_definitions(
  DEBUG
  GLM_ENABLE_EXPERIMENTAL
  )

add_executable(TinyGame ${HEADERS} ${SOURCES})

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

set(GAME_SRC_INCLUDE ${PROJECT_SOURCE_DIR}/TinyGame/src/)
set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLFW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)
set(BULLET_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)
set(SNDFILE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/include/)

target_include_directories(TinyGame
PUBLIC ${GAME_SRC_INCLUDE}
PUBLIC ${ENGINE_CORE_INCLUDE}
PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
PUBLIC ${LOGER_DEPENDENCE_INCLUDE}
PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
PUBLIC ${GLM_DEPENDENCE_INCLUDE}
PUBLIC ${BULLET_DEPENDENCE_INCLUDE}
PUBLIC ${BULLET_PHYS_INCLUDE}
PUBLIC ${SNDFILE_INCLUDE}
)

if (UNIX)
	target_link_libraries(TinyGame PRIVATE X11)
	target_link_libraries(TinyGame PRIVATE Threads::Threads)

	find_library(GLFW_LIBRARIES glfw3 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/Unix/)
	if (NOT GLFW_LIBRARIES)
		message(FATAL_ERROR "GLFW libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC ${GLFW_LIBRARIES})
	endif()

	if (NOT CMAKE_DL_LIBS)
   	 	message(FATAL_ERROR "DL library wasn't found!")
  	else()
    	target_link_libraries(TinyGame PUBLIC ${CMAKE_DL_LIBS})
	endif()

	find_library(GLEW_LIBRARIES GLEW ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
	if (NOT GLEW_LIBRARIES)
		message(FATAL_ERROR "GLEW libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC ${GLEW_LIBRARIES})
	endif()

	find_library(LUA_LIBRARIES lua ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
	if (NOT LUA_LIBRARIES)
		message(FATAL_ERROR "Lua libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC ${LUA_LIBRARIES})
	endif()

	find_library(ASSIMP_LIBRARIES assimp ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
	if (NOT ASSIMP_LIBRARIES)
		message(FATAL_ERROR "Assimp libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC ${ASSIMP_LIBRARIES})
	endif()

	find_library(SNDFILE_LIBRARIES sndfile ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/Unix/)
	if (NOT SNDFILE_LIBRARIES)
		message(FATAL_ERROR "SndFile libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC ${SNDFILE_LIBRARIES})
	endif()

	find_library(BULLET3COLLISION_LIBRARIES Bullet3Collision ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETCOLLISION_LIBRARIES BulletCollision ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLET3COMMON_LIBRARIES Bullet3Common ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLET3DYNAMICS_LIBRARIES Bullet3Dynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLET3GEOMETRY_LIBRARIES Bullet3Geometry ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETDYNAMICS_LIBRARIES BulletDynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETLINEARMATH_LIBRARIES LinearMath ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETINVERSEDYNAMICS_LIBRARIES BulletInverseDynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(CONVEXDECOMPOSITION_LIBRARIES ConvexDecomposition ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETROBOTICS_LIBRARIES BulletRobotics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	find_library(BULLETSOFTBODY_LIBRARIES BulletSoftBody ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
	message(STATUS "Bullet3Collision = ${BULLET3COLLISION_LIBRARIES} \n 
	BulletCollision = ${BULLETCOLLISION_LIBRARIES} \n 
	Bullet3Common = ${BULLET3COMMON_LIBRARIES} \n 
	Bullet3Dynamics = ${BULLET3DYNAMICS_LIBRARIES} \n 
	Bullet3Geometry = ${BULLET3GEOMETRY_LIBRARIES} \n 
	BulletDynamics = ${BULLETDYNAMICS_LIBRARIES} \n 
	LinearMath = ${BULLETLINEARMATH_LIBRARIES} \n 
	BulletInverseDynamics = ${BULLETINVERSEDYNAMICS_LIBRARIES} \n 
	ConvexDecomposition = ${CONVEXDECOMPOSITION_LIBRARIES} \n 
	BulletRobotics = ${BULLETROBOTICS_LIBRARIES} \n 
	BulletSoftBody = ${BULLETSOFTBODY_LIBRARIES} \n 
	")
	if (NOT BULLET3DYNAMICS_LIBRARIES 
    	OR NOT BULLET3COLLISION_LIBRARIES
		OR NOT BULLETCOLLISION_LIBRARIES
    	OR NOT BULLET3COMMON_LIBRARIES
    	OR NOT BULLET3GEOMETRY_LIBRARIES 
    	OR NOT BULLETDYNAMICS_LIBRARIES 
    	OR NOT BULLETLINEARMATH_LIBRARIES
		OR NOT BULLETINVERSEDYNAMICS_LIBRARIES
		OR NOT CONVEXDECOMPOSITION_LIBRARIES
		OR NOT BULLETROBOTICS_LIBRARIES
		OR NOT BULLETSOFTBODY_LIBRARIES)
		  message(FATAL_ERROR "BulletPhysics libraries were not found!")
	else()
		target_link_libraries(TinyGame PUBLIC
			PUBLIC ${BULLETCOLLISION_LIBRARIES}
			PUBLIC ${BULLET3COLLISION_LIBRARIES}
			PUBLIC ${BULLET3DYNAMICS_LIBRARIES}
			PUBLIC ${BULLETDYNAMICS_LIBRARIES}
			PUBLIC ${BULLET3COMMON_LIBRARIES}
			PUBLIC ${BULLET3GEOMETRY_LIBRARIES}
			PUBLIC ${BULLETLINEARMATH_LIBRARIES}
			PUBLIC ${BULLETINVERSEDYNAMICS_LIBRARIES}
			PUBLIC ${CONVEXDECOMPOSITION_LIBRARIES}
			PUBLIC ${BULLETROBOTICS_LIBRARIES}
			PUBLIC ${BULLETSOFTBODY_LIBRARIES}
     )
	endif()
endif(UNIX)

target_link_libraries(TinyGame 
PRIVATE ${OPENGL_LIBRARIES}
PRIVATE TinyLogger
PRIVATE PHEngine
)
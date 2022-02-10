set(
	HEADERS
	src/GameLevelFactory.h
	src/IntroLevel.h
)

set(
	SOURCES
	src/GameLevelFactory.cpp
	src/IntroLevel.cpp
    main.cpp
)

add_compile_definitions(
  DEBUG
  )

add_executable(TinyGame ${HEADERS} ${SOURCES})

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLFW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)
set(BULLET_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)

target_include_directories(TinyGame
PUBLIC ${ENGINE_CORE_INCLUDE}
PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
PUBLIC ${LOGER_DEPENDENCE_INCLUDE}
PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
PUBLIC ${GLM_DEPENDENCE_INCLUDE}
PUBLIC ${BULLET_DEPENDENCE_INCLUDE}
PUBLIC ${BULLET_PHYS_INCLUDE}
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
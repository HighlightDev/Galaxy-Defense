add_compile_definitions(
  GLM_ENABLE_EXPERIMENTAL
  SHADING_MODEL_PBR
  DEBUG
  NO_LIT
  )

file(GLOB_RECURSE SOURCES
  "*.cpp"
)

file (GLOB_RECURSE HEADERS 
  "*.h")

add_library(PHEngine STATIC ${SOURCES} ${HEADERS})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(LUA_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/include/)
set(BULLET_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(SNDFILE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/include/)

target_include_directories(PHEngine 
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${LOGER_DEPENDENCE_INCLUDE}
  PRIVATE ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PRIVATE ${GLM_DEPENDENCE_INCLUDE}
  PRIVATE ${CEREAL_DEPENDENCE_INCLUDE}
  PRIVATE ${GLEW_DEPENDENCE_INCLUDE}
  PRIVATE ${BULLET_DEPENDENCE_INCLUDE}
  PRIVATE ${BULLET_PHYS_INCLUDE}
  PUBLIC ${LUA_DEPENDENCE_INCLUDE}
  PUBLIC ${SNDFILE_INCLUDE}
  )

if (UNIX)	
  target_link_libraries(PHEngine PRIVATE X11)
  target_link_libraries(PHEngine PRIVATE Threads::Threads)

  find_library(GLFW_LIBRARIES glfw3 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/Unix/)
  if (NOT GLFW_LIBRARIES)
    message(FATAL_ERROR "GLFW libraries were not found!")
  else()
    target_link_libraries(PHEngine 
      PUBLIC ${GLFW_LIBRARIES}
      )
  endif()

  if (NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(PHEngine
      PUBLIC ${CMAKE_DL_LIBS}
      )
  endif()

	find_library(GLEW_LIBRARIES GLEW ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
	if (NOT GLEW_LIBRARIES)
		message(FATAL_ERROR "GLEW libraries were not found!")
	else()
		target_link_libraries(PHEngine PUBLIC ${GLEW_LIBRARIES})
	endif()

	find_library(LUA_LIBRARIES lua ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
	if (NOT LUA_LIBRARIES)
		message(FATAL_ERROR "Lua libraries were not found!")
	else()
		target_link_libraries(PHEngine PUBLIC ${LUA_LIBRARIES})
	endif()

  find_library(ASSIMP_LIBRARIES assimp ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
	if (NOT ASSIMP_LIBRARIES)
		message(FATAL_ERROR "Assimp libraries were not found!")
	else()
		target_link_libraries(PHEngine PUBLIC ${ASSIMP_LIBRARIES})
	endif()

  find_library(SNDFILE_LIBRARIES sndfile ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/Unix/)
	if (NOT SNDFILE_LIBRARIES)
		message(FATAL_ERROR "SndFile libraries were not found!")
	else()
		target_link_libraries(PHEngine PUBLIC ${SNDFILE_LIBRARIES})
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
		target_link_libraries(PHEngine
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

target_link_libraries(PHEngine 
PUBLIC ${OPENGL_LIBRARIES}
PUBLIC ${OPENAL_LIBRARY}
PUBLIC TinyLogger
)
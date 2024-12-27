add_compile_definitions(
  GLM_ENABLE_EXPERIMENTAL
  SHADING_MODEL_PBR
  DEBUG
  #NO_LIT
)

file(GLOB_RECURSE SOURCES
  "*.cpp"
)

file(GLOB_RECURSE HEADERS
  "*.h")

add_library(PHEngine STATIC ${SOURCES} ${HEADERS})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(LUA_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/include/)
set(NLOHMANN_JSON_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/json/nlohmann/include/)
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
  PUBLIC ${NLOHMANN_JSON_INCLUDE}
)

if(WIN32)
  message("ARCH_x64: ${ARCH_x64}")
  if(ARCH_x64)
    set(LIB_LOCATION_PREFIX Win/x64/)
  else()
    set(LIB_LOCATION_PREFIX Win/x86/)
  endif(ARCH_x64)

  find_library(OPENAL_LIBRARY OpenAL32 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/OpenAL/lib/${LIB_LOCATION_PREFIX})
  if(NOT OPENAL_LIBRARY)
    message(FATAL_ERROR "OPENAL_LIBRARY wasn't found!")
  endif()

  set(OPENAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/OpenAL/include)

  find_library(GLEW_LIBRARIES glew32 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/${LIB_LOCATION_PREFIX}/Release/)
  if(NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "GLEW libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${GLEW_LIBRARIES})
  endif()

  find_library(LUA_LIBRARIES lua54 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/${LIB_LOCATION_PREFIX}/Debug/)
  if(NOT LUA_LIBRARIES)
    message(FATAL_ERROR "Lua libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${LUA_LIBRARIES})
  endif()

  find_library(ASSIMP_LIBRARIES assimp-vc143-mtd ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/${LIB_LOCATION_PREFIX}/Debug/)
  if(NOT ASSIMP_LIBRARIES)
    message(FATAL_ERROR "Assimp libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${ASSIMP_LIBRARIES})
  endif()

  find_library(BULLET3COLLISION_LIBRARIES Bullet3Collision ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETCOLLISION_LIBRARIES BulletCollision ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLET3COMMON_LIBRARIES Bullet3Common ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLET3DYNAMICS_LIBRARIES Bullet3Dynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLET3GEOMETRY_LIBRARIES Bullet3Geometry ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETDYNAMICS_LIBRARIES BulletDynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETLINEARMATH_LIBRARIES LinearMath ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETINVERSEDYNAMICS_LIBRARIES BulletInverseDynamics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(CONVEXDECOMPOSITION_LIBRARIES ConvexDecomposition ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETROBOTICS_LIBRARIES BulletRobotics ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)
  find_library(BULLETSOFTBODY_LIBRARIES BulletSoftBody ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)

  if(NOT BULLET3DYNAMICS_LIBRARIES
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


  find_library(SNDFILE_LIBRARIES sndfile ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/${LIB_LOCATION_PREFIX}/Debug/)
  if(NOT SNDFILE_LIBRARIES)
    message(FATAL_ERROR "SndFile libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${SNDFILE_LIBRARIES})
  endif()

  find_library(GLFW_LIBRARIES glfw3 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/${LIB_LOCATION_PREFIX}/)
  if(NOT GLFW_LIBRARIES)
    message(FATAL_ERROR "GLFW libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${GLFW_LIBRARIES})
  endif()

  target_include_directories(PHEngine PUBLIC ${OPENAL_INCLUDE_DIR})
elseif(UNIX)
  set(LIB_LOCATION_PREFIX Unix)
  target_link_libraries(PHEngine PRIVATE X11)
  target_link_libraries(PHEngine PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(PHEngine
      PUBLIC ${CMAKE_DL_LIBS}
    )
  endif()

  find_library(GLEW_LIBRARIES GLEW ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
  if(NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "GLEW libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${GLEW_LIBRARIES})
  endif()

  find_library(LUA_LIBRARIES lua ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
  if(NOT LUA_LIBRARIES)
    message(FATAL_ERROR "Lua libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${LUA_LIBRARIES})
  endif()

  find_library(ASSIMP_LIBRARIES assimp ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
  if(NOT ASSIMP_LIBRARIES)
    message(FATAL_ERROR "Assimp libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${ASSIMP_LIBRARIES})
  endif()

  find_library(SNDFILE_LIBRARIES sndfile ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/Unix/)
  if(NOT SNDFILE_LIBRARIES)
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

  if(NOT BULLET3DYNAMICS_LIBRARIES
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

  find_library(GLFW_LIBRARIES glfw3 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/Unix/)
  if(NOT GLFW_LIBRARIES)
    message(FATAL_ERROR "GLFW libraries were not found!")
  else()
    target_link_libraries(PHEngine PUBLIC ${GLFW_LIBRARIES})
  endif()

endif()

target_link_libraries(PHEngine
  PUBLIC ${OPENGL_LIBRARIES}
  PUBLIC ${OPENAL_LIBRARY}
  PUBLIC TinyLogger
)

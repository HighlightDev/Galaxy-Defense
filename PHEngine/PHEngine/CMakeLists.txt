
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Utility)

add_compile_definitions(
  GLM_ENABLE_EXPERIMENTAL
  SHADING_MODEL_PBR
  DEBUG
  #NO_LIT
)

file(GLOB_RECURSE SOURCES
  ${PROJECT_SOURCE_DIR}/PHEngine/src/*.cpp)

file(GLOB_RECURSE HEADERS
  ${PROJECT_SOURCE_DIR}/PHEngine/src/*.h)

add_library(PHEngine STATIC ${SOURCES} ${HEADERS})

set(ENGINE_DEPENDENCIES_DIR ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/GLEW/include/)
set(LUA_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/Lua54/include/)
set(NLOHMANN_JSON_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/json/nlohmann/include/)
set(BULLET_DEPENDENCE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(SNDFILE_INCLUDE ${ENGINE_DEPENDENCIES_DIR}/SndFile/include/)

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

  set(OPENAL_INCLUDE_DIR ${ENGINE_DEPENDENCIES_DIR}/OpenAL/include)
  target_include_directories(PHEngine PUBLIC ${OPENAL_INCLUDE_DIR})
  find_library(OPENAL_LIBRARY OpenAL32 ${ENGINE_DEPENDENCIES_DIR}/OpenAL/lib/${LIB_LOCATION_PREFIX})
  if(NOT OPENAL_LIBRARY)
    message(FATAL_ERROR "OPENAL_LIBRARY wasn't found!")
  endif()

  findLibraryAndLinkToTarget(PHEngine PUBLIC glew32 ${ENGINE_DEPENDENCIES_DIR}/GLEW/${LIB_LOCATION_PREFIX}/Release/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC lua54 ${ENGINE_DEPENDENCIES_DIR}/Lua54/lib/${LIB_LOCATION_PREFIX}/Debug/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC assimp-vc143-mtd ${ENGINE_DEPENDENCIES_DIR}/Assimp/lib/${LIB_LOCATION_PREFIX}/Debug/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC sndfile ${ENGINE_DEPENDENCIES_DIR}/SndFile/lib/${LIB_LOCATION_PREFIX}/Debug/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC glfw3 ${ENGINE_DEPENDENCIES_DIR}/GLFW/lib/${LIB_LOCATION_PREFIX}/)
  findLibrariesAndLinkToTarget(PHEngine PUBLIC BULLET_LIBS ${ENGINE_DEPENDENCIES_DIR}/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/)

elseif(UNIX)

  target_link_libraries(PHEngine PRIVATE X11)
  target_link_libraries(PHEngine PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(PHEngine
      PUBLIC ${CMAKE_DL_LIBS}
    )
  endif()

  findLibraryAndLinkToTarget(PHEngine PUBLIC assimp ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC lua ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC GLEW ${ENGINE_DEPENDENCIES_DIR}/GLEW/lib/Unix/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC sndfile ${ENGINE_DEPENDENCIES_DIR}/SndFile/lib/Unix/)
  findLibraryAndLinkToTarget(PHEngine PUBLIC glfw3 ${ENGINE_DEPENDENCIES_DIR}/GLFW/lib/Unix/)
  findLibrariesAndLinkToTarget(PHEngine PUBLIC BULLET_LIBS ${ENGINE_DEPENDENCIES_DIR}/BulletPhysics/lib/Unix/Release/)

endif()

target_link_libraries(PHEngine
  PUBLIC ${OPENGL_LIBRARIES}
  PUBLIC ${OPENAL_LIBRARY}
  PUBLIC TinyLogger
)

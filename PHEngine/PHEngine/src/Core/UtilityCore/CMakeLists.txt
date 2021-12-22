set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/AssimpToGlmConverter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/EngineMath.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GlmToBulletConverter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/PlatformDependentFunctions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/StringExtendedFunctions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/StringStreamWrapper.h
)

set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/AssimpToGlmConverter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/EngineMath.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GlmToBulletConverter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PlatformDependentFunctions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StringExtendedFunctions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StringStreamWrapper.cpp
)

add_library(EngineUtilityCoreModule STATIC ${HEADERS} ${SOURCE})

set_target_properties(EngineUtilityCoreModule PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(EngineUtilityCoreModule 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE})

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

target_include_directories(EngineUtilityCoreModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

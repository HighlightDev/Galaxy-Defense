set(HEADERS
  Assertion.h
  CommonMeta.h
  StringHash.h
  VariableWrapper.h
  XMLParserHelper.h
)

add_library(EngineCommonCoreModule STATIC ${HEADERS})

set_target_properties(EngineCommonCoreModule PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(STB_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/STB/include/)
set(ASSIMP_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)

target_include_directories(EngineCommonCoreModule 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${STB_DEPENDENCE_INCLUDE}
  PUBLIC ${ASSIMP_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE})
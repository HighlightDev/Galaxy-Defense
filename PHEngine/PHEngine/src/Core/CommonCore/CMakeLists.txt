set(HEADERS
  Assertion.h
  CommonMeta.h
  StringHash.h
  VariableWrapper.h
  XMLParserHelper.h
)

add_library(EngineCommonCoreModule STATIC ${HEADERS})

set_target_properties(EngineCommonCoreModule PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(EngineCommonCoreModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
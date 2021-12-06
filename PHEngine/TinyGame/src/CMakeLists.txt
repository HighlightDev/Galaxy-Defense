
set(
	HEADERS
	LabyrinthLevelFactory.h
	SimpleLevel.h
)

set(
	SOURCE
	LabyrinthLevelFactory.cpp
	SimpleLevel.cpp
)

add_library(TinyGameSrcModule STATIC ${HEADERS} ${SOURCE})

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)

message(STATUS "ENGINE_CORE_INCLUDE path = ${ENGINE_CORE_INCLUDE}")

target_include_directories(TinyGameSrcModule
 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
 PUBLIC ${ENGINE_CORE_INCLUDE})
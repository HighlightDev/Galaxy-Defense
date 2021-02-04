
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

target_include_directories(TinyGameSrcModule INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
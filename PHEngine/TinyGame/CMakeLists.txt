set(
	HEADERS
	src/LabyrinthLevelFactory.h
	src/SimpleLevel.h
)

set(
	SOURCES
	src/LabyrinthLevelFactory.cpp
	src/SimpleLevel.cpp
    main.cpp
)

add_executable(TinyGame ${HEADERS} ${SOURCES})

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/External/glfw glfw_build)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(EXTERNAL_DEPENDENCIES_INCLUDE ${PROJECT_SOURCE_DIR}/External/)
set(LOGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(CEREAL_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/cereal/include/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)

target_include_directories(TinyGame
PUBLIC ${ENGINE_CORE_INCLUDE}
PUBLIC ${EXTERNAL_DEPENDENCIES_INCLUDE}
PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
PUBLIC ${LOGER_DEPENDENCE_INCLUDE}
PUBLIC ${CEREAL_DEPENDENCE_INCLUDE}
PUBLIC ${GLM_DEPENDENCE_INCLUDE}
)

if (UNIX)
	find_library(GLEW_LIBRARIES glew ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
	target_link_libraries(TinyGame 
		PUBLIC glew)
endif(UNIX)

target_link_libraries(TinyGame 
PUBLIC glfw
PUBLIC TinyLogger
PUBLIC PHEngine
)
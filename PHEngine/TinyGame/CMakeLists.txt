add_subdirectory(src)

set(APPEND STATIC_LIBS TinyGameSrcModule PHEngine)

add_executable(TinyGame main.cpp)

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(EXTERNAL_DEPENDENCIES_INCLUDE ${PROJECT_SOURCE_DIR}/External/)
set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLFW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)

message(STATUS "GLEW directory path=${GLFW_DEPENDENCE_INCLUDE}")

target_include_directories(TinyGame
    PUBLIC ${ENGINE_CORE_INCLUDE}
    PUBLIC ${EXTERNAL_DEPENDENCIES_INCLUDE}
    PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
    PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
    )

target_link_libraries(TinyGame ${STATIC_LIBS})
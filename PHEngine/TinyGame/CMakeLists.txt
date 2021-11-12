add_subdirectory(src)

set(APPEND STATIC_LIBS TinyGameSrcModule PHEngine)

add_executable(TinyGame main.cpp)

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(EXTERNAL_DEPENDENCIES_INCLUDE ${PROJECT_SOURCE_DIR}/External/)

target_include_directories(TinyGame
    PUBLIC ${ENGINE_CORE_INCLUDE}
    PUBLIC ${EXTERNAL_DEPENDENCIES_INCLUDE}
    )

target_link_libraries(TinyGame ${STATIC_LIBS})
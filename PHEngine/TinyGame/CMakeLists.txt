add_subdirectory(src)

set(APPEND STATIC_LIBS TinyGameSrcModule PHEngine)

add_executable(TinyGame Main.cpp)

target_link_libraries(TinyGame ${STATIC_LIBS})
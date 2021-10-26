add_subdirectory(src)

set(APPEND STATIC_LIBS TinyGameSrcModule PHEngine)

add_executable(TinyGame Main.cpp)

set_target_properties(TinyGame PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(TinyGame ${STATIC_LIBS})
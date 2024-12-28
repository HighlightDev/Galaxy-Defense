set(LIBRARY_NAME UnwindTool)

set(SOURCES
    sources/TinyUnwinder.cpp
)

set(HEADERS
    includes/TinyUnwinder.h
)

add_library(${LIBRARY_NAME} SHARED ${HEADERS} ${SOURCES})

add_compile_definitions(
    UNW_LOCAL_ONLY #remove this if remote unwinding is necessary
)

set(UNWIND_TOOL_INCLUDE ${PROJECT_SOURCE_DIR}/UnwindTool/includes/)

target_include_directories(${LIBRARY_NAME}
    PUBLIC ${UNWIND_TOOL_INCLUDE}
)

find_library(LIBUNWIND_LIB libunwind.so /usr/lib/)
find_library(LIBUNWIND_PTRACE_LIB libunwind-ptrace.so /usr/lib/)
if(NOT LIBUNWIND_LIB
    OR NOT LIBUNWIND_PTRACE_LIB)
    message(FATAL_ERROR "Libunwind libraries were not found!")
else()
    message(STATUS "Libunwind libraries were found!")
    target_link_libraries(${LIBRARY_NAME}
        PRIVATE ${LIBUNWIND_LIB}
        PRIVATE ${LIBUNWIND_PTRACE_LIB})
endif()

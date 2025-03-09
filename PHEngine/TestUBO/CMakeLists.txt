set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/PHEngine/cmake)
include(Utility)

set(
    SOURCES
    main.cpp
    UniformBuffer.cpp
)

set(
    HEADERS
    UniformBuffer.h
)

add_compile_definitions(
    DEBUG
)

set(EXECUTABLE_NAME TestUBO)
add_executable(${EXECUTABLE_NAME} ${SOURCES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(GLEW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLFW_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)

target_include_directories(${EXECUTABLE_NAME}
    PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
    PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
    PUBLIC ${GLM_DEPENDENCE_INCLUDE}
)

message("ARCH_x64: ${ARCH_x64}")
if(ARCH_x64)
    set(LIB_LOCATION_PREFIX Win/x64/)
else()
    set(LIB_LOCATION_PREFIX Win/x86/)
endif(ARCH_x64)

findLibraryAndLinkToTarget(${EXECUTABLE_NAME} PUBLIC glew32 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/${LIB_LOCATION_PREFIX}/Release/)
findLibraryAndLinkToTarget(${EXECUTABLE_NAME} PUBLIC glfw3 ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/${LIB_LOCATION_PREFIX}/)

target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE ${OPENGL_LIBRARIES}
    # Link against legacy_stdio_definitions.lib on Windows
    $<$<PLATFORM_ID:Windows>:legacy_stdio_definitions>
)
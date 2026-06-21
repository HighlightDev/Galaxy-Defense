set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/PHEngine/cmake)
include(Utility)

set(HEADERS
    src/Implementation/Levels/TestFeaturesLevelFactory.h
    src/Implementation/Levels/TestFeaturesLevel.h
    src/Implementation/Levels/Controllers/TestFeaturesLevelUiController.h
    src/Implementation/LuaExecutors/LuaUiControllerExecutor.h)

set(SOURCES
    src/Implementation/Levels/TestFeaturesLevelFactory.cpp
    src/Implementation/Levels/TestFeaturesLevel.cpp
    src/Implementation/Levels/Controllers/TestFeaturesLevelUiController.cpp
    src/Implementation/LuaExecutors/LuaUiControllerExecutor.cpp
    main.cpp)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

if(DEBUG_BUILD)
  add_compile_definitions(DEBUG)
endif()

set(EXECUTABLE_NAME EngineTest)
add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})

add_custom_command(
  TARGET ${EXECUTABLE_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Start copy of resources to build directory: $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res"
)

add_custom_command(
  TARGET ${EXECUTABLE_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/PHEngine/res/
          $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(GAME_SRC_INCLUDE ${PROJECT_SOURCE_DIR}/${EXECUTABLE_NAME}/src/)
set(ENGINE_CORE_INCLUDE ${PROJECT_SOURCE_DIR}/PHEngine/src/)
set(LOGGER_DEPENDENCE_INCLUDE ${PROJECT_SOURCE_DIR}/Logger/)
set(UNWIND_TOOL_INCLUDE ${PROJECT_SOURCE_DIR}/UnwindTool/includes/)
set(GLEW_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/include/)
set(GLM_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLM/include/)
set(GLFW_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/include/)
set(NLOHMANN_JSON_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/json/nlohmann/include/)
set(BULLET_DEPENDENCE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/include/)
set(BULLET_PHYS_INCLUDE ${BULLET_DEPENDENCE_INCLUDE}/BulletPhys/)
set(SNDFILE_INCLUDE
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/include/)

target_include_directories(
  ${EXECUTABLE_NAME}
  PUBLIC ${GAME_SRC_INCLUDE}
  PUBLIC ${ENGINE_CORE_INCLUDE}
  PUBLIC ${GLEW_DEPENDENCE_INCLUDE}
  PUBLIC ${GLFW_DEPENDENCE_INCLUDE}
  PUBLIC ${LOGGER_DEPENDENCE_INCLUDE}
  PUBLIC ${GLM_DEPENDENCE_INCLUDE}
  PUBLIC ${BULLET_DEPENDENCE_INCLUDE}
  PUBLIC ${BULLET_PHYS_INCLUDE}
  PUBLIC ${SNDFILE_INCLUDE}
  PUBLIC ${NLOHMANN_JSON_INCLUDE})

set(BULLET_LIBS
    Bullet3Collision
    BulletCollision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Geometry
    BulletDynamics
    LinearMath
    ConvexDecomposition)

if(WIN32)

  add_custom_command(
  TARGET ${EXECUTABLE_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Copy dll files to binary directory: $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/r"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/bin/Debug/
          ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/FreeType/bin/Release/
          ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/bin/Release/x64/
          ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/bin/Debug/
          $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/)

  if(ARCH_x64)
    set(LIB_LOCATION_PREFIX Win/x64/)
  else()
    set(LIB_LOCATION_PREFIX Win/x86/)
  endif(ARCH_x64)

  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    glew32
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/${LIB_LOCATION_PREFIX}/Release/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    lua54
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    assimp-vc143-mtd
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    sndfile
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC glfw3
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/${LIB_LOCATION_PREFIX}/
  )
  findlibrariesandlinktotarget(
    ${EXECUTABLE_NAME}
    PUBLIC
    BULLET_LIBS
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/${LIB_LOCATION_PREFIX}/Debug/
  )
elseif(UNIX)
  set(LIB_LOCATION_PREFIX "Unix")

  target_link_libraries(${EXECUTABLE_NAME} PRIVATE X11)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)

  if(NOT CMAKE_DL_LIBS)
    message(FATAL_ERROR "DL library wasn't found!")
  else()
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CMAKE_DL_LIBS})
  endif()

  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC assimp
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Assimp/lib/Unix/Release/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC lua
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/Lua54/lib/Unix/Release/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC GLEW
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLEW/lib/Unix/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC sndfile
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/SndFile/lib/Unix/)
  findlibraryandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC glfw3
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/GLFW/lib/Unix/)
  findlibrariesandlinktotarget(
    ${EXECUTABLE_NAME} PUBLIC BULLET_LIBS
    ${PROJECT_SOURCE_DIR}/PHEngine/Dependencies/BulletPhysics/lib/Unix/Release/)
endif()

if(ENABLE_ASAN)
  message(STATUS "ASAN enabled for ${EXECUTABLE_NAME}")
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=address -g -O1
                                                    -fno-omit-frame-pointer)
  target_link_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=address)

  target_compile_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=undefined -g -O1)
  target_link_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=undefined)
endif()

if(ENABLE_ASAN AND ENABLE_TSAN)
  message(
    WARNING
      "ASAN enabled for ${EXECUTABLE_NAME}, Thread sanitizer will be disabled")
elseif(ENABLE_TSAN)
  message(STATUS "TSAN enabled for ${EXECUTABLE_NAME}")
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=thread -g -O1)
  target_link_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=thread)
endif()

target_link_libraries(
  ${EXECUTABLE_NAME}
  PRIVATE ${OPENGL_LIBRARIES}
  PRIVATE TinyLogger
  PRIVATE PHEngine)

message(STATUS "LibUnwind enabled: ${ENABLE_LIBUNWIND}")
if(ENABLE_LIBUNWIND)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE UnwindTool)
else()

endif(ENABLE_LIBUNWIND)

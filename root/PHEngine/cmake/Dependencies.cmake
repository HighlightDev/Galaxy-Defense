find_package(OpenGL REQUIRED)

if(UNIX)
  find_package(X11 REQUIRED)
  find_package(Threads REQUIRED)
endif()

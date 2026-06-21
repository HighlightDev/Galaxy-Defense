
if(UNIX)
set(ProfilerCompilationFlags
    -O2
    -g
    -fno-omit-frame-pointer)
elseif(WIN32)
    set(ProfilerCompilationFlags
    -O2
    -g)
endif()

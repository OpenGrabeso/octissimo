cmake_minimum_required(VERSION 3.15)
project(Octissimo)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")

# searching for include directory
find_path(WINHTTP_INCLUDE_DIR Winhttp.h)

# searching for library file
find_library(WINHTTP_LIBRARY Winhttp)

if (WINHTTP_INCLUDE_DIR AND WINHTTP_LIBRARY)
    # you may need that if further action in your CMakeLists.txt depends
    # on detecting your library
    set(WINHTTP_FOUND TRUE)

    # you may need that if you want to conditionally compile some parts
    # of your code depending on library availability
    # add_definitions(-DHAVE_LIBSIFTGPU=1)

    # those two, you really need
    include_directories(${WINHTTP_INCLUDE_DIR})
    set(YOUR_LIBRARIES ${YOUR_LIBRARIES} ${WINHTTP_LIBRARY})
endif ()

# Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
set_source_files_properties(${CMAKE_SOURCE_DIR}/res.rc LANGUAGE RC)
# For MinGW, we have to change the compile flags
if(MINGW)
    # resource compilation for mingw
    set(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
    SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
    set(RC_CFLAGS "-DMINGW -Ocoff")
    # If any sort of debugging is being enabled, add a _DEBUG define to the flags for the resource compiler
    #if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
    #   set(RC_CFLAGS "${RC_CFLAGS} -D_DEBUG")
    #endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/res.rc COMPILE_FLAGS "${RC_CFLAGS}")
    # For anything else, assumingly Visual Studio at this point, use a different set of compile flags
endif(MINGW)

add_executable(Octissimo WIN32 main.cpp resource.rc GithubStatus.cpp GithubStatus.h resource.h)
target_link_libraries(Octissimo ${YOUR_LIBRARIES})
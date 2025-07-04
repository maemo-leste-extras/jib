cmake_minimum_required(VERSION 3.16)
project(jib)
message(STATUS "Initiating compile using CMake ${CMAKE_VERSION}")

option(STATIC "Link libraries statically, requires static Qt")

# add cmake/ directory as cmake module path
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

# why not
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_CXX_STANDARD 17)
include(FindCcache)

include(FindPkgConfig)
pkg_search_module(LIBHILDON hildon-1)
if(DEFINED LIBHILDON_LIBRARIES)
    set(MAEMO ON)
endif()

# Find X11
if(UNIX AND NOT APPLE AND NOT ANDROID)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_SKIP_RPATH ON)
    endif()

    find_package(X11 REQUIRED)
    message(STATUS "X11_FOUND = ${X11_FOUND}")
    message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

    include_directories(${X11_INCLUDE_DIR})
    link_directories(${X11_LIBRARIES})
    if(STATIC)
        find_library(XCB_LIBRARY xcb)
        message(STATUS "Found xcb library: ${XCB_LIBRARY}")
    endif()
endif()

find_package(Threads REQUIRED)

add_subdirectory(src)
install(FILES jib.desktop DESTINATION share/applications/hildon)

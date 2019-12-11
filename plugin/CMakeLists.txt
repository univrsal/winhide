project(winhide)

set(NETLIB_INCLUDE_DIR "../deps/netlib/include")

if (MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(NETLIB_LIBRARIES "../deps/netlib/win64/netlib.lib")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(NETLIB_LIBRARIES "../deps/netlib/win32/netlib.lib")
    endif()
    add_definitions(-DWINDOWS=1)
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(NETLIB_LIBRARIES "../deps/netlib/linux64/libnetlib.so")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(NETLIB_LIBRARIES "../deps/netlib/linux32/libnetlib.so")
    endif()
    add_definitions(-DUNIX=1)
    add_definitions(-DLINUX=1)
endif()

set(winhide_SOURCES
    util.hpp
    winhide.cpp
    server.cpp
    server.hpp
    source.hpp
    source.cpp)

add_library(winhide MODULE
    ${winhide_SOURCES})
target_link_libraries(winhide
    libobs
    ${NETLIB_LIBRARIES})
include_directories(${NETLIB_INCLUDE_DIR})
install_obs_plugin_with_data(winhide data)

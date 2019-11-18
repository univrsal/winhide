project(winhide C)
cmake_minimum_required(VERSION 2.8)

set(WINHIDE_SRC
	src/main.c
	src/util/window_list.c
	src/util/window_list.h
	src/util/config.c
	src/util/config.h
	src/network/network.c
	src/network/network.h)

find_path(NETLIB_HEADERS netlib.h)
find_library(NETLIB_LIBRARIES netlib)

find_path(JANSSON_HEADERS jansson.h)
find_library(JANSSON_LIBRARIES jansson)

set(CMAKE_C_STANDARD 99)
set(CMAKE_WIN32_EXECTUABLE true)

include_directories(${NETLIB_HEADERS} ${JANSSON_HEADERS})
add_executable("winhide" "${WINHIDE_SRC}")
target_link_libraries(winhide ${NETLIB_LIBRARIES} ${JANSSON_LIBRARIES})
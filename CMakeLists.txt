cmake_minimum_required(VERSION 3.10)

project(main)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib-5.0_linux_amd64)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.c"
        )

include_directories(${RAYLIB_DIR}/include)
find_library(RAYLIB_LIBRARY raylib HINTS ${RAYLIB_DIR}/lib)

add_executable(${PROJECT_NAME} ${all_SRCS})

target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} m GL pthread dl rt X11)

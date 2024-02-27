cmake_minimum_required(VERSION 3.10)

project(${PROJECT_NAME})

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_linux_amd64)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_win64_mingw-w64)
else()
    message(FATAL_ERROR "Operating system not supported: ${CMAKE_SYSTEM_NAME}")
endif()


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


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} m GL pthread dl rt X11)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} m pthread opengl32 gdi32 winmm)
else()
    message(FATAL_ERROR "Operating system not supported: ${CMAKE_SYSTEM_NAME}")
endif()

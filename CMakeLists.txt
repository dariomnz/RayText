cmake_minimum_required(VERSION 3.10)

project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(RAYLIB_LIBRARY ${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_linux_amd64/lib/libraylib.a)
        include_directories(${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_linux_amd64/include)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(RAYLIB_LIBRARY ${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_win64_mingw-w64/lib/libraylib.a)
        include_directories(${CMAKE_SOURCE_DIR}/raylib/raylib-5.0_win64_mingw-w64/include)
else()
    message(FATAL_ERROR "Operating system not supported: ${CMAKE_SYSTEM_NAME}")
endif()


include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
        )


add_executable(${PROJECT_NAME} ${all_SRCS})


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} m GL pthread dl rt X11)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} m pthread opengl32 gdi32 winmm)
else()
    message(FATAL_ERROR "Operating system not supported: ${CMAKE_SYSTEM_NAME}")
endif()

cmake_minimum_required(VERSION 3.14)
project(WinBGChanger)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 20)
add_definitions(-DWINVER=_WIN32_WINNT_WIN10 -D_WIN32_WINNT=_WIN32_WINNT_WIN10)

if(NOT DEFINED PLATFORM)
	set(PLATFORM "." CACHE STRING "current platform")
endif()
set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "CMake install prefix" FORCE)
endif()

include_directories(
	include
	"${ToolsSY_ROOT}/include"
)

#find_library(Dependency_LIB lib_name "${Dependency_ROOT}/lib/${BUILD_SFX}")

add_executable(WinBGChanger
	src/main.cc
)

#target_link_libraries(WinBGChanger
#	"${Dependency_LIB}"
#)

install(TARGETS
	WinBGChanger
DESTINATION "bin/${BUILD_SFX}")

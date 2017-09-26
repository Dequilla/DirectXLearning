cmake_minimum_required(VERSION 3.4)

project(DirectX C CXX)

# Add modulepath 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

# Find DirectX:
#
#  DirectX_XYZ_INCLUDE_FOUND - system has the include for the XYZ API
#  DirectX_XYZ_INCLUDE_DIR   - include directory for the XYZ API
#
# Where XYZ can be any of:
#
#  DDRAW
#  D3D
#  D3D8
#  D3D9
#  D3D10
#  D3D10_1
#  D3D11
#  D3D11_1
#  D3D11_2
#  D2D1
#
find_package(DirectX REQUIRED)

# TODO: Check if found
include_directories(DirectX_D3D_INCLUDE_DIR)
include_directories(DirectX_D3D9_INCLUDE_DIR)
include_directories(DirectX_D3D10_INCLUDE_DIR)
include_directories(DirectX_D3D11_INCLUDE_DIR)

set(SOURCE_FILES 
		${CMAKE_SOURCE_DIR}/src/Main.cpp
		${CMAKE_SOURCE_DIR}/src/Utility.h
		${CMAKE_SOURCE_DIR}/src/Window.cpp)

if(WIN32)
	add_executable(DirectX WIN32 ${SOURCE_FILES})
else()
	add_executable(DirectX ${SOURCE_FILES})
endif()

# Debug C/C++ flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUGMODE -D_DEBUG)
else()
  add_definitions(-DNDEBUG)
endif()
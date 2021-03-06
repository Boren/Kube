# Set CMake version number
cmake_minimum_required(VERSION 3.1)

# Set project name and version
project(Kube)
set(KUBE_VERSION_MAJOR 0)
set(KUBE_VERSION_MINOR 1)

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
  # use -Wall for gcc
  IF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall$")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  ENDIF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall$")

  IF(NOT CMAKE_C_FLAGS MATCHES "-Wall$")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  ENDIF(NOT CMAKE_C_FLAGS MATCHES "-Wall$")
ENDIF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)

# Limit the supported build configurations
set (KUBE_BUILD_CONFIGURATIONS Release Debug)

find_package(Doxygen)

option (KUBE_BUILD_SAMPLES "Build sample applications" TRUE)
option (KUBE_BUILD_DOCUMENTATION "Create HTML based engine documentation (Requires Doxygen)" ${DOXYGEN_FOUND})

# Define target name
set (TARGET_NAME Kube)

# Flags
# TODO: Figure out why we need this, GLFW is not happy without
add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-D_CRT_SECURE_NO_WARNINGS
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# GLFW
find_package(GLFW 3.2 REQUIRED)

# Freetype
find_package(Freetype 2.7 REQUIRED)

# Source files
set(KUBE_SOURCE_FILES
	src/kubeEngine.cpp
	src/kubeEngine.h
	src/kubeWindow.cpp
	src/kubeWindow.h

	src/chunks/block.cpp
	src/chunks/block.h
	src/chunks/blocktype.cpp
	src/chunks/blocktype.h
	src/chunks/chunk.cpp
	src/chunks/chunk.h
	src/chunks/chunkManager.cpp
	src/chunks/chunkManager.h

	src/input/inputManager.cpp
	src/input/inputManager.h

  src/renderer/material.cpp
  src/renderer/material.h
  src/renderer/mesh.cpp
  src/renderer/mesh.h
	src/renderer/renderer.cpp
	src/renderer/renderer.h
	src/renderer/shader.cpp
	src/renderer/shader.h
  src/renderer/textManager.cpp
  src/renderer/textManager.h
	src/renderer/texture.cpp
	src/renderer/texture.h

  src/scene/camera.cpp
  src/scene/camera.h
  src/scene/light.h
  src/scene/scenemanager.cpp
  src/scene/scenemanager.h

  src/utils/color.h
	src/utils/log.cpp
  src/utils/log.h)

add_library(Kube src/glad.c ${KUBE_SOURCE_FILES})

include_directories(include/ src/ ${GLFW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(Kube ${GLFW_LIBRARIES} ${FREETYPE_LIBRARIES})


IF(KUBE_BUILD_SAMPLES)
	add_subdirectory(samples)
ENDIF()

IF(KUBE_BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
			message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile} @ONLY)

	add_custom_target(doc
			COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM)

	#install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
ENDIF()

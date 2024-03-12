cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/externals/_vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_ARCHITECTURE x64)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(libax5/src/libax_common.cmake)

#set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_NAME "ax_anon")


if (CMAKE_GENERATOR_TOOLSET)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_TOOLSET}")
endif()
if (CMAKE_GENERATOR_PLATFORM)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_PLATFORM}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/deploy/bin/${VCPKG_TARGET_TRIPLET}")

message("==== ${PROJECT_NAME} ====")
message("VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")
project(${PROJECT_NAME})

add_subdirectory(libax5/src/ax_detect_platform)
add_subdirectory(libax5/src/ax_core)
add_subdirectory(libax5/src/ax_core/unit_test)

add_subdirectory(libax5/src/ax_render)
add_subdirectory(libax5/src/ax_ui)

add_subdirectory(libax5/tools/axFileCmd)
add_subdirectory(libax5/tools/axNodeGen)
add_subdirectory(libax5/tools/axRenderVertexGen)
add_subdirectory(libax5/tools/axShaderCompiler)

add_subdirectory(anon/libAnon)
add_subdirectory(anon/libAnonEditor)
add_subdirectory(anon/AnonEditor)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "AnonEditor")



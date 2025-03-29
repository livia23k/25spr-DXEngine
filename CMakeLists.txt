# ----------------------------------------------------
# Version
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------
# Project
project(CelestialRover
    VERSION 0.1.0
    LANGUAGES CXX
)

# ----------------------------------------------------
# Platform Detection
if(WIN32)
    set(PLATFORM "Windows")
    set(RENDERER "DX11")
    add_definitions(-DPLATFORM_WINDOWS)
endif()

message(STATUS "Building for ${PLATFORM} using ${RENDERER}")

# ----------------------------------------------------
# Dependencies
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}") # Managed by vcpkg
endif()

# Windows: DirectX 11
if(WIN32)
    include(cmake/FindDirectX11.cmake)

    if(DirectX11_FOUND)
        message(STATUS "DirectX 11 found: ${DirectX11_INCLUDE_DIRS}")

        add_definitions(-DUSE_DIRECTX11)
        include_directories(${DirectX11_INCLUDE_DIRS})
        list(APPEND SRC_FILES engine/source/graphics/graphics_dx11.cpp)
        list(APPEND LIBS ${DirectX11_LIBRARIES})
    else()
        message(FATAL_ERROR "DirectX 11 NOT found!")
    endif()

    set(DIRECTX_TEX_DIR "${CMAKE_SOURCE_DIR}/engine/include/external/DirectXTex")
    include_directories("${DIRECTX_TEX_DIR}")
    add_library(DirectXTex STATIC IMPORTED)
    set_target_properties(DirectXTex PROPERTIES
        IMPORTED_LOCATION_RELEASE "${DIRECTX_TEX_DIR}/DirectXTex.lib"
        IMPORTED_LOCATION_DEBUG "${DIRECTX_TEX_DIR}/DirectXTex_debug.lib"
    )
endif()

# ----------------------------------------------------
# Source Files
file(GLOB_RECURSE SRC_FILES
    engine/source/*.cpp
    game/*/source/*.cpp
)
file(GLOB_RECURSE HEADER_FILES
    engine/include/*.h
    game/*/include/*.h
)

# message(STATUS "SRC_FILES GLOB RESULT:")
# foreach(f ${SRC_FILES})
# message(STATUS "  ${f}")
# endforeach()

# ----------------------------------------------------
# Build Executable
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
    add_executable(CelestialRover WIN32 ${SRC_FILES} ${HEADER_FILES})
else()
    add_executable(CelestialRover ${SRC_FILES} ${HEADER_FILES})
endif()

target_link_libraries(CelestialRover PRIVATE ${LIBS} DirectXTex)
target_include_directories(CelestialRover PRIVATE
    engine/include
    game/celestial_rover/include
)

# ----------------------------------------------------
# Build Options
if(MSVC)
    target_compile_options(CelestialRover PRIVATE /W4)
else()
    target_compile_options(CelestialRover PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Find DirectX 11 Directory
file(GLOB WIN_SDK_VERSIONS_PATH "C:/Program Files (x86)/Windows Kits/10/Include/10.*")
list(SORT WIN_SDK_VERSIONS_PATH ORDER DESCENDING)
list(GET WIN_SDK_VERSIONS_PATH 0 WIN_SDK_LATEST_PATH)
get_filename_component(WIN_SDK_VERSION ${WIN_SDK_LATEST_PATH} NAME)

message(STATUS "Using Windows SDK Version: ${WIN_SDK_VERSION}")

set(WIN_SDK_INCLUDE "C:/Program Files (x86)/Windows Kits/10/Include/${WIN_SDK_VERSION}/um")
set(WIN_SDK_LIB "C:/Program Files (x86)/Windows Kits/10/Lib/${WIN_SDK_VERSION}/um/x64")

# Find DirectX 11 headfiles
find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h PATHS
    "${WIN_SDK_INCLUDE}"
)

# Find DirectX 11 libraries
find_library(D3D11_LIBRARY NAMES d3d11 PATHS "${WIN_SDK_LIB}")
find_library(DXGI_LIBRARY NAMES dxgi PATHS "${WIN_SDK_LIB}")
find_library(D3DCOMPILER_LIBRARY NAMES d3dcompiler d3dcompiler_47 PATHS "${WIN_SDK_LIB}")

# Check if all libraries are found
if (DirectX11_INCLUDE_DIR AND D3D11_LIBRARY AND DXGI_LIBRARY AND D3DCOMPILER_LIBRARY)
    set(DirectX11_FOUND TRUE)
    set(DirectX11_INCLUDE_DIRS ${DirectX11_INCLUDE_DIR})
    set(DirectX11_LIBRARIES ${D3D11_LIBRARY} ${DXGI_LIBRARY} ${D3DCOMPILER_LIBRARY})
else()
    set(DirectX11_FOUND FALSE)
    message(FATAL_ERROR "DirectX 11 not found. Make sure Windows SDK is installed.")
endif()

# Make variables known to CMake GUI
mark_as_advanced(DirectX11_INCLUDE_DIR DirectX11_LIBRARIES)

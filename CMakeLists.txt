cmake_minimum_required(VERSION 3.20)
project(XboxMultiGenEmulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Core Libraries ---
add_library(core_common INTERFACE)
target_include_directories(core_common INTERFACE core/include)

# Xbox Original
add_library(core_xbox_original STATIC core_xbox_original/src/*.cpp)
target_link_libraries(core_xbox_original PUBLIC core_common)
target_include_directories(core_xbox_original PUBLIC core_xbox_original/include)

# Xbox 360
add_library(core_xbox_360 STATIC core_xbox_360/src/*.cpp)
target_link_libraries(core_xbox_360 PUBLIC core_common)
target_include_directories(core_xbox_360 PUBLIC core_xbox_360/include)

# Xbox One
add_library(core_xbox_one STATIC core_xbox_one/src/*.cpp)
target_link_libraries(core_xbox_one PUBLIC core_common)
target_include_directories(core_xbox_one PUBLIC core_xbox_one/include)

# Xbox Series
add_library(core_xbox_series STATIC core_xbox_series/src/*.cpp)
target_link_libraries(core_xbox_series PUBLIC core_common)
target_include_directories(core_xbox_series PUBLIC core_xbox_series/include)

# --- 2. Windows Host & D3D11 ---
add_library(host_win STATIC host_win/src/*.cpp)
target_link_libraries(host_win PUBLIC core_common core_xbox_original core_xbox_360 core_xbox_one core_xbox_series)
target_include_directories(host_win PUBLIC host_win/include)

if(WIN32)
    target_link_libraries(host_win PRIVATE d3d11 dxgi d3dcompiler xinput user32 gdi32)
endif()

# --- 3. Main Executable ---
add_executable(XboxEmulator host_win/src/main.cpp)
target_link_libraries(XboxEmulator PRIVATE host_win)
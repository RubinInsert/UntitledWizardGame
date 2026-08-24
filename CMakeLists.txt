cmake_minimum_required(VERSION 3.15)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Auto-bootstrap vcpkg
if(WIN32)
    set(VCPKG_EXE "${CMAKE_SOURCE_DIR}/vcpkg/vcpkg.exe")
    set(VCPKG_BOOTSTRAP "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat")
else()
    set(VCPKG_EXE "${CMAKE_SOURCE_DIR}/vcpkg/vcpkg")
    set(VCPKG_BOOTSTRAP "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh")
endif()

if(NOT EXISTS "${VCPKG_EXE}")
    message(STATUS "vcpkg executable not found. Bootstrapping...")
    execute_process(COMMAND "${VCPKG_BOOTSTRAP}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vcpkg")
endif()

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(MyEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Third-Party Packages
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(box3d REQUIRED)
find_package(EnTT REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

include(cmake/Shaders.cmake)

# ------------------------------------------------------------------------------
# 1. SHARED ENGINE CORE LIBRARY (Excludes main entry points)
# ------------------------------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES 
    CONFIGURE_DEPENDS
    "src/engine/*.cpp"
    "src/game/*.cpp"
    "src/editor/*.cpp"
)

add_library(EngineCore STATIC ${ENGINE_SOURCES})

target_include_directories(EngineCore PUBLIC 
    src
    include
)

target_link_libraries(EngineCore PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    EnTT::EnTT
    nlohmann_json::nlohmann_json
    assimp::assimp
    box3d::box3d
)

if(SHADER_TARGET)
    add_dependencies(EngineCore ${SHADER_TARGET})
endif()

# ------------------------------------------------------------------------------
# 2. GAME TARGET
# ------------------------------------------------------------------------------
add_executable(MyGame "src/main_game.cpp")
target_link_libraries(MyGame PRIVATE EngineCore)

# ------------------------------------------------------------------------------
# 3. EDITOR TARGET
# ------------------------------------------------------------------------------
add_executable(MyEditor "src/main_editor.cpp")
target_link_libraries(MyEditor PRIVATE EngineCore)

# ------------------------------------------------------------------------------
# 4. POST-BUILD ASSETS COPY
# ------------------------------------------------------------------------------
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Syncing assets to build directory"
)
add_dependencies(MyGame CopyAssets)
add_dependencies(MyEditor CopyAssets)
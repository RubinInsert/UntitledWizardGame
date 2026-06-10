cmake_minimum_required(VERSION 3.15)

# 1. OPTIONAL: Auto-bootstrap vcpkg if it's not ready
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

# 2. Set the Toolchain File automatically
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# 3. Define Project
project(MySDLGame LANGUAGES CXX)

# Use C++17 (standard for modern SDL development)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Find SDL3
# vcpkg makes this package available via 'find_package'
 find_package(SDL3 CONFIG REQUIRED)
 find_package(SDL3_shadercross CONFIG REQUIRED)
 find_package(SDL3_image REQUIRED)
 find_package(EnTT REQUIRED)
 find_package(nlohmann_json CONFIG REQUIRED)
# 5. Create the executable
# Notice we point to src/main.cpp because of your new folder structure
add_executable(${PROJECT_NAME}
 src/main.cpp
 src/engine/core/Engine.cpp
 src/engine/core/WindowManager.cpp
 src/engine/core/AssetManager.cpp
 src/engine/core/InputManager.cpp
 src/engine/core/TimeManager.cpp
 src/engine/core/AnimatorSystem.cpp
 src/engine/render/RenderSystem.cpp
 src/engine/render/SpriteRenderer.cpp
 src/engine/render/SpriteSheet.cpp
 src/game/core/Game.cpp
 src/game/core/CameraSystem.cpp
 src/game/player/PlayerController.cpp
 src/game/movement/PhysicsSystem.cpp
 src/game/world/TileMap.cpp
 src/game/movement/CollisionSystem.cpp
 src/game/world/MapLoader.cpp
 src/game/world/TileSet.cpp
 )

# 6. Link SDL3
# SDL3::SDL3 is the main library
# SDL3::SDL3main is required for the entry point (the main function)
target_link_libraries(${PROJECT_NAME} PRIVATE
                    SDL3::SDL3
                    SDL3_shadercross::SDL3_shadercross
                    SDL3_image::SDL3_image
                    EnTT::EnTT
                    nlohmann_json::nlohmann_json)

# 7. Include the src folder so headers are easy to find later
target_include_directories(${PROJECT_NAME} PRIVATE 
                            src
                            include)


# Copy Assets to output directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Syncing assets to build directory"
)
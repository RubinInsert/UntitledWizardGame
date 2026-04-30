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

# 4. Find SDL2
# vcpkg makes this package available via 'find_package'
 find_package(SDL3 CONFIG REQUIRED)
# 5. Create the executable
# Notice we point to src/main.cpp because of your new folder structure
add_executable(${PROJECT_NAME} src/main.cpp)

# 6. Link SDL2
# SDL2::SDL2 is the main library
# SDL2::SDL2main is required for the entry point (the main function)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# 7. Include the src folder so headers are easy to find later
target_include_directories(${PROJECT_NAME} PRIVATE src)
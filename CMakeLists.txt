cmake_minimum_required(VERSION 3.15)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)


# Auto-bootstrap vcpkg if it's not ready
if(WIN32)
    set(VCPKG_EXE "${CMAKE_SOURCE_DIR}/vcpkg/vcpkg.exe")
    set(VCPKG_BOOTSTRAP "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat")
else()
    set(VCPKG_EXE "${CMAKE_SOURCE_DIR}/vcpkg/vcpkg")
    set(VCPKG_BOOTSTRAP "${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh")
endif()

# On First Setup, if vcpkg isn't bootstrapped, do it automatically (for easy portability and setup)
if(NOT EXISTS "${VCPKG_EXE}")
    message(STATUS "vcpkg executable not found. Bootstrapping...")
    execute_process(COMMAND "${VCPKG_BOOTSTRAP}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vcpkg")
endif()

# Set the Toolchain File automatically
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# 3. Define Project
project(MySDLGame LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Find Third Party Libraries
 find_package(SDL3 CONFIG REQUIRED)
 #find_package(SDL3_shadercross CONFIG REQUIRED)
 find_package(SDL3_image REQUIRED)
 find_package(box3d REQUIRED)
 find_package(EnTT REQUIRED)
 find_package(nlohmann_json CONFIG REQUIRED)
 find_package(assimp CONFIG REQUIRED)
# Include additional CMake modules
include(cmake/Shaders.cmake) # Custom module to handle shader compilation


# 5. Create the executable
# Notice we point to src/main.cpp because of your new folder structure
file(GLOB_RECURSE SOURCE_FILES 
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.c"
    "src/*.cc"
)

# Remove the manual list
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 6. Link SDL3
# SDL3::SDL3 is the main library
# SDL3::SDL3main is required for the entry point (the main function)
target_link_libraries(${PROJECT_NAME} PRIVATE
                    SDL3::SDL3
                    #SDL3_shadercross::SDL3_shadercross
                    SDL3_image::SDL3_image
                    EnTT::EnTT
                    nlohmann_json::nlohmann_json
                    assimp::assimp
                    box3d::box3d
                    )

# 7. Include the src folder so headers are easy to find later
target_include_directories(${PROJECT_NAME} PRIVATE 
                            src
                            include)

compile_all_shaders(SHADER_TARGET)
if(SHADER_TARGET)
    add_dependencies(${PROJECT_NAME} ${SHADER_TARGET})
    message(STATUS "Added shader compilation dependency: ${SHADER_TARGET}")
endif()
# Copy Assets to output directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Syncing assets to build directory"
)
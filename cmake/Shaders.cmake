# cmake/Shaders.cmake
# Handles automatic shader compilation from HLSL to SPIR-V and DXIL

# Find required tools
find_program(SHADERCROSS_TOOL shadercross REQUIRED)
find_program(DXC_TOOL dxc REQUIRED)

# Function to compile all shaders and return the target
function(compile_all_shaders SHADER_TARGET_VAR)
    set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders")
    
    if(NOT EXISTS ${SHADER_DIR})
        message(WARNING "Shader directory not found: ${SHADER_DIR}")
        return()
    endif()
    
    # Find all shader files
    file(GLOB_RECURSE SHADER_SOURCES 
        CONFIGURE_DEPENDS  
        "${SHADER_DIR}/*.vert.hlsl"
        "${SHADER_DIR}/*.frag.hlsl"
        "${SHADER_DIR}/*.comp.hlsl"
    )
    
    if(NOT SHADER_SOURCES)
        message(STATUS "No shader files found in ${SHADER_DIR}")
        set(${SHADER_TARGET_VAR} "" PARENT_SCOPE)
        return()
    endif()
    
    message(STATUS "Found ${SHADER_SOURCES} shader files")
    
    set(ALL_OUTPUTS "")
    
    foreach(SHADER_SOURCE ${SHADER_SOURCES})
        get_filename_component(SHADER_FULLNAME ${SHADER_SOURCE} NAME)
        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)
        
        # Determine shader type
        if(${SHADER_FULLNAME} MATCHES "\.vert\.")
            set(SHADER_STAGE "vert")
            set(DXIL_PROFILE "vs_6_0")
        elseif(${SHADER_FULLNAME} MATCHES "\.frag\.")
            set(SHADER_STAGE "frag")
            set(DXIL_PROFILE "ps_6_0")
        elseif(${SHADER_FULLNAME} MATCHES "\.comp\.")
            set(SHADER_STAGE "comp")
            set(DXIL_PROFILE "cs_6_0")
        else()
            continue()
        endif()
        
        set(OUTPUT_BASENAME "${SHADER_NAME}.${SHADER_STAGE}")
        
        # SPIR-V output
        set(SPIRV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/SPIRV/${OUTPUT_BASENAME}.spv")
        add_custom_command(
            OUTPUT ${SPIRV_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/SPIRV"
            COMMAND ${SHADERCROSS_TOOL} ${SHADER_SOURCE} -o ${SPIRV_OUTPUT}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_FULLNAME} -> SPIR-V"
        )
        list(APPEND ALL_OUTPUTS ${SPIRV_OUTPUT})
        
        # DXIL output
        set(DXIL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/DXIL/${OUTPUT_BASENAME}.dxil")
        add_custom_command(
            OUTPUT ${DXIL_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/DXIL"
            COMMAND ${DXC_TOOL} -T ${DXIL_PROFILE} -E main -Fo ${DXIL_OUTPUT} ${SHADER_SOURCE}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_FULLNAME} -> DXIL"
        )
        list(APPEND ALL_OUTPUTS ${DXIL_OUTPUT})
    endforeach()
    
    # Create the target
    set(SHADER_TARGET_NAME "CompileShaders")
    add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${ALL_OUTPUTS})
    
    # Return the target name
    set(${SHADER_TARGET_VAR} ${SHADER_TARGET_NAME} PARENT_SCOPE)
endfunction()
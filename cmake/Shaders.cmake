# cmake/Shaders.cmake
# Compiles HLSL shaders directly to SPIR-V and DXIL using DXC

find_program(DXC_TOOL dxc)

function(compile_all_shaders SHADER_TARGET_VAR)
    set(${SHADER_TARGET_VAR} "" PARENT_SCOPE)

    if(NOT DXC_TOOL)
        message(WARNING "DXC compiler tool not found. Skipping shader compilation.")
        return()
    endif()

    set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders")
    
    if(NOT EXISTS ${SHADER_DIR})
        message(WARNING "Shader directory not found: ${SHADER_DIR}")
        return()
    endif()
    
    file(GLOB_RECURSE SHADER_SOURCES 
        CONFIGURE_DEPENDS  
        "${SHADER_DIR}/*.vert.hlsl"
        "${SHADER_DIR}/*.frag.hlsl"
        "${SHADER_DIR}/*.comp.hlsl"
    )
    
    if(NOT SHADER_SOURCES)
        message(STATUS "No shader files found in ${SHADER_DIR}")
        return()
    endif()
    
    set(ALL_OUTPUTS "")
    
    foreach(SHADER_SOURCE ${SHADER_SOURCES})
        get_filename_component(SHADER_FULLNAME ${SHADER_SOURCE} NAME)
        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)
        
        if(${SHADER_FULLNAME} MATCHES "\.vert\.")
            set(SHADER_STAGE "vert")
            set(DXC_PROFILE "vs_6_0")
        elseif(${SHADER_FULLNAME} MATCHES "\.frag\.")
            set(SHADER_STAGE "frag")
            set(DXC_PROFILE "ps_6_0")
        elseif(${SHADER_FULLNAME} MATCHES "\.comp\.")
            set(SHADER_STAGE "comp")
            set(DXC_PROFILE "cs_6_0")
        else()
            continue()
        endif()
        
        set(OUTPUT_BASENAME "${SHADER_NAME}.${SHADER_STAGE}")
        
        # 1. DXC -> SPIR-V (uses -spirv flag)
        set(SPIRV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/SPIRV/${OUTPUT_BASENAME}.spv")
        add_custom_command(
            OUTPUT ${SPIRV_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/SPIRV"
            COMMAND ${DXC_TOOL} -spirv -T ${DXC_PROFILE} -E main -Fo ${SPIRV_OUTPUT} ${SHADER_SOURCE}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_FULLNAME} -> SPIR-V"
        )
        list(APPEND ALL_OUTPUTS ${SPIRV_OUTPUT})
        
        # 2. DXC -> DXIL
        set(DXIL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/DXIL/${OUTPUT_BASENAME}.dxil")
        add_custom_command(
            OUTPUT ${DXIL_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/compiled/DXIL"
            COMMAND ${DXC_TOOL} -T ${DXC_PROFILE} -E main -Fo ${DXIL_OUTPUT} ${SHADER_SOURCE}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Compiling ${SHADER_FULLNAME} -> DXIL"
        )
        list(APPEND ALL_OUTPUTS ${DXIL_OUTPUT})
    endforeach()
    
    set(SHADER_TARGET_NAME "CompileShaders")
    add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${ALL_OUTPUTS})
    
    set(${SHADER_TARGET_VAR} ${SHADER_TARGET_NAME} PARENT_SCOPE)
endfunction()
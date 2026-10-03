#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP
#include <SDL3/SDL_gpu.h>

// Auto-detects stage from the filename (.vert / .frag) and loads the correct
// backend blob (SPIRV / MSL / DXIL) from assets/shaders/compiled/<api>/.
namespace ShaderLoader {
    SDL_GPUShader* Load(
        SDL_GPUDevice* device,
        const char* shaderFilename,
        Uint32 samplerCount        = 0,
        Uint32 uniformBufferCount  = 0,
        Uint32 storageBufferCount  = 0,
        Uint32 storageTextureCount = 0);
}
#endif
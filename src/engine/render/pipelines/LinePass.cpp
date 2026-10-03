#include "engine/render/pipelines/LinePass.hpp"
#include "engine/render/ShaderLoader.hpp"
#include <cstring>

bool LinePass::init(SDL_GPUDevice* dev, SDL_Window* window) {
    device = dev;
    return createPipeline(window);
}

bool LinePass::createPipeline(SDL_Window* window) {
    SDL_GPUShader* vertShader = ShaderLoader::Load(device, "line.vert", 0, 1, 0, 0);
    SDL_GPUShader* fragShader = ShaderLoader::Load(device, "line.frag", 0, 0, 0, 0);
    if (!vertShader || !fragShader) return false;

    SDL_GPUVertexBufferDescription vbDesc{
        0, sizeof(LineVertex), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0 };
    SDL_GPUVertexAttribute attrs[2] = {
        {0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(LineVertex, position)},
        {1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(LineVertex, color)},
    };
    SDL_GPUVertexInputState vin{ &vbDesc, 1, attrs, 2 };

    SDL_GPURasterizerState raster{
        SDL_GPU_FILLMODE_FILL, SDL_GPU_CULLMODE_NONE,
        SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        0, 0, 0, false, false, 0, 0 };

    SDL_GPUMultisampleState ms{ SDL_GPU_SAMPLECOUNT_1, 0, false, false, 0, 0 };
    SDL_GPUStencilOpState stencil{
        SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_KEEP,
        SDL_GPU_STENCILOP_KEEP, SDL_GPU_COMPAREOP_ALWAYS };

    // Depth test on, depth WRITE off -> grid blends over geometry, hidden behind it
    SDL_GPUDepthStencilState depth{
        SDL_GPU_COMPAREOP_LESS, stencil, stencil, 0xFF, 0xFF,
        true,   // depth_test_enabled
        false,  // depth_write_enabled
        false, 0, 0, 0 };

    SDL_GPUColorTargetBlendState blend{
        SDL_GPU_BLENDFACTOR_SRC_ALPHA, SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        SDL_GPU_BLENDOP_ADD,
        SDL_GPU_BLENDFACTOR_ONE, SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        SDL_GPU_BLENDOP_ADD,
        SDL_GPU_COLORCOMPONENT_R|SDL_GPU_COLORCOMPONENT_G|
        SDL_GPU_COLORCOMPONENT_B|SDL_GPU_COLORCOMPONENT_A,
        true,       // enable_blend
        false, 0, 0 };

    SDL_GPUColorTargetDescription colorDesc{
        SDL_GetGPUSwapchainTextureFormat(device, window), blend };
    SDL_GPUGraphicsPipelineTargetInfo target{
        &colorDesc, 1, SDL_GPU_TEXTUREFORMAT_D32_FLOAT, true, 0, 0, 0 };

    SDL_GPUGraphicsPipelineCreateInfo info{
        vertShader, fragShader, vin,
        SDL_GPU_PRIMITIVETYPE_LINELIST,
        raster, ms, depth, target, 0 };

    pipeline = SDL_CreateGPUGraphicsPipeline(device, &info);
    SDL_ReleaseGPUShader(device, vertShader);
    SDL_ReleaseGPUShader(device, fragShader);
    if (!pipeline) SDL_Log("LinePass pipeline failed: %s", SDL_GetError());
    return pipeline != nullptr;
}

void LinePass::submit(const glm::vec3& a, const glm::vec3& b, const SDL_FColor& color) {
    vertices.push_back({a, color});
    vertices.push_back({b, color});
    dirty = true;
}

void LinePass::ensureCapacity(size_t vertexCount) {
    if (vertexCount <= vertexCapacity && vertexBuffer) return;
    if (vertexBuffer) SDL_ReleaseGPUBuffer(device, vertexBuffer);
    vertexCapacity = vertexCount * 2;   // headroom
    SDL_GPUBufferCreateInfo bi{ SDL_GPU_BUFFERUSAGE_VERTEX,
                                (Uint32)(vertexCapacity * sizeof(LineVertex)) };
    vertexBuffer = SDL_CreateGPUBuffer(device, &bi);
}

void LinePass::prepare(SDL_GPUCommandBuffer* cmd) {
    if (!dirty || vertices.empty()) return;
    ensureCapacity(vertices.size());
    if (!vertexBuffer) return;

    Uint32 bytes = (Uint32)(vertices.size() * sizeof(LineVertex));
    SDL_GPUTransferBufferCreateInfo ti{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, bytes };
    SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &ti);
    void* mapped = SDL_MapGPUTransferBuffer(device, tb, false);
    std::memcpy(mapped, vertices.data(), bytes);
    SDL_UnmapGPUTransferBuffer(device, tb);

    SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTransferBufferLocation src{ tb, 0 };
    SDL_GPUBufferRegion dst{ vertexBuffer, 0, bytes };
    SDL_UploadToGPUBuffer(cp, &src, &dst, true);   // cycle = true
    SDL_EndGPUCopyPass(cp);
    SDL_ReleaseGPUTransferBuffer(device, tb);
    dirty = false;
}

void LinePass::draw(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass,
                    Camera& camera, Engine&) {
    if (!pipeline || vertices.empty()) return;
    SDL_BindGPUGraphicsPipeline(pass, pipeline);

    struct { glm::mat4 model, view, projection; } ubo;
    ubo.model = glm::mat4(1.0f);                       // positions are already world-space
    ubo.view  = camera.getViewMatrix();
    ubo.projection = camera.getProjectionMatrix();
    SDL_PushGPUVertexUniformData(cmd, 0, &ubo, sizeof(ubo));

    SDL_GPUBufferBinding vb{ vertexBuffer, 0 };
    SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
    SDL_DrawGPUPrimitives(pass, (Uint32)vertices.size(), 1, 0, 0);
}

void LinePass::shutdown(SDL_GPUDevice* dev) {
    if (pipeline) SDL_ReleaseGPUGraphicsPipeline(dev, pipeline);
    if (vertexBuffer) SDL_ReleaseGPUBuffer(dev, vertexBuffer);
}
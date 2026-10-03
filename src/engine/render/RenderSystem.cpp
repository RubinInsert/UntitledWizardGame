#include "engine/render/RenderSystem.hpp"
#include "engine/render/Texture.hpp"
#include "engine/core/Engine.h"
#include "engine/render/pipelines/MeshPass.hpp"
#include "engine/render/pipelines/LinePass.hpp"
#include <entt/entt.hpp>
#include <engine/ecs/components/MeshComponent.hpp>
#include <engine/ecs/components/Transform.hpp>
RenderSystem::RenderSystem() {
    // Leave empty for now or initialize pointers to nullptr
}
RenderSystem::RenderSystem(SDL_GPUDevice* device) {
    this->device = device;
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::setGPUDevice(SDL_GPUDevice* device) { this->device = device; }
void RenderSystem::setTargetWindow(SDL_Window* window) { targetWindow = window; }

void RenderSystem::initResources(int width, int height, Engine& eng) {
    engine = &eng;

    meshPass = std::make_unique<MeshPass>();
    meshPass->init(device, targetWindow);

    linePass = std::make_unique<LinePass>();
    linePass->init(device, targetWindow);

    passOrder = { meshPass.get(), linePass.get() };
    resourcesInitialized = true;
    

    resourcesInitialized = true;
}
bool RenderSystem::SubmitMesh(Mesh* mesh, const Transform& transform) {
    return meshPass->submit(mesh, transform);
}
void RenderSystem::SubmitDebugLine(const glm::vec3& a, const glm::vec3& b, const SDL_FColor& color) {
    if (linePass) linePass->submit(a, b, color);
}
void RenderSystem::ClearDebugLines() {
    if (linePass) linePass->clear();
}
// New public method — renders to an off-screen target
void RenderSystem::renderToTarget(RenderTarget& target) {
    if (!resourcesInitialized) return;

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) return;

    // Prepare color + depth target info from the RenderTarget
    SDL_GPUColorTargetInfo colorTarget{};
    SDL_GPUDepthStencilTargetInfo depthTarget{};
    target.getTargetInfo(colorTarget, depthTarget,
                         0.1f, 0.1f, 0.2f, 1.0f); // clear color
    
    prepareAll(cmd);

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, &depthTarget);
    if (!pass) {
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    // Adjust camera aspect to match the render target, not the window
    float originalAspect = camera.aspectRatio;
    camera.aspectRatio = (float)target.getWidth() / (float)target.getHeight();

    drawAll(cmd, pass);

    camera.aspectRatio = originalAspect; // restore

    SDL_EndGPURenderPass(pass);
    SDL_SubmitGPUCommandBuffer(cmd);
}

void RenderSystem::render() {
    if (!resourcesInitialized) return;

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) return;

    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, targetWindow, &swapchainTex, &w, &h)) {
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    camera.aspectRatio = (float)w / (float)h;

    // Depth texture management
    static SDL_GPUTexture* depthTexture = nullptr;
    static Uint32 depthW = 0, depthH = 0;
    if (!depthTexture || depthW != w || depthH != h) {
        if (depthTexture) SDL_ReleaseGPUTexture(device, depthTexture);
        SDL_GPUTextureCreateInfo depthInfo{};
        depthInfo.type = SDL_GPU_TEXTURETYPE_2D;
        depthInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
        depthInfo.width = w;
        depthInfo.height = h;
        depthInfo.layer_count_or_depth = 1;
        depthInfo.num_levels = 1;
        depthInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        depthTexture = SDL_CreateGPUTexture(device, &depthInfo);
        depthW = w; depthH = h;
    }

    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTex;
    colorTarget.clear_color = {0.1f, 0.1f, 0.2f, 1.0f};
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = depthTexture;
    depthTarget.clear_depth = 1.0f;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

    prepareAll(cmd);   // copy-pass uploads BEFORE the render pass

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, &depthTarget);
    if (!pass) {
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    drawAll(cmd, pass);

    SDL_EndGPURenderPass(pass);
    SDL_SubmitGPUCommandBuffer(cmd);
}
void RenderSystem::prepareAll(SDL_GPUCommandBuffer* cmd) {
    for (auto* p : passOrder) p->prepare(cmd);
}
void RenderSystem::drawAll(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass) {
    for (auto* p : passOrder) p->draw(cmd, pass, camera, *engine);
}
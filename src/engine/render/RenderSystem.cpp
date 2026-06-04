#include "engine/render/RenderSystem.hpp"
RenderSystem::RenderSystem() {
    // Leave empty for now or initialize pointers to nullptr
}
RenderSystem::RenderSystem(SDL_GPUDevice* device) {
    this->device = device;
}

RenderSystem::~RenderSystem() {
    // Cleanup code later
}

void RenderSystem::initResources(int width, int height) {
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;

    heightBuffer = SDL_CreateGPUTexture(device, &textureInfo);
    shadowBuffer = SDL_CreateGPUTexture(device, &textureInfo);
    
    resourcesInitialized = true;
}
void RenderSystem::setGPUDevice(SDL_GPUDevice* device) {
    this->device = device;
}
void RenderSystem::setTargetWindow(SDL_Window* window) {
    targetWindow = window;
}
void RenderSystem::render() {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if(!cmd) return;

    // Run render passes
    runHeightMapPass(cmd);
    runShadowPass(cmd);

    runAlbedoPass(cmd);

    SDL_SubmitGPUCommandBuffer(cmd);
}

void RenderSystem::runHeightMapPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runShadowPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runAlbedoPass(SDL_GPUCommandBuffer* cmd) {
    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if(SDL_WaitAndAcquireGPUSwapchainTexture (cmd, targetWindow, &swapchainTex, &w, &h)) {
        // Define the color target info
        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = swapchainTex;
        colorTargetInfo.clear_color = { 0.39f, 0.58f, 0.93f, 1.0f };; // Black background
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    //     // Begin the pass that draws to the screen
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, nullptr);

    //     // Render stuff?

        
        SDL_EndGPURenderPass(renderPass);
    }
}
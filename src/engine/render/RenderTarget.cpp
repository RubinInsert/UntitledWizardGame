#include "engine/render/RenderTarget.hpp"
#include <SDL3/SDL.h>
#include <cstring>

RenderTarget::RenderTarget(SDL_GPUDevice* device, int width, int height)
    : mDevice(device), mWidth(width), mHeight(height) {
    createTextures();
}

RenderTarget::~RenderTarget() {
    destroyTextures();
}

RenderTarget::RenderTarget(RenderTarget&& other) noexcept
    : mDevice(other.mDevice)
    , mColorTexture(other.mColorTexture)
    , mDepthTexture(other.mDepthTexture)
    , mWidth(other.mWidth)
    , mHeight(other.mHeight) {
    other.mColorTexture = nullptr;
    other.mDepthTexture = nullptr;
    other.mDevice = nullptr;
    other.mWidth = 0;
    other.mHeight = 0;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other) noexcept {
    if (this != &other) {
        destroyTextures();
        mDevice = other.mDevice;
        mColorTexture = other.mColorTexture;
        mDepthTexture = other.mDepthTexture;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        other.mColorTexture = nullptr;
        other.mDepthTexture = nullptr;
        other.mDevice = nullptr;
        other.mWidth = 0;
        other.mHeight = 0;
    }
    return *this;
}

void RenderTarget::createTextures() {
    // --- Color target texture ---
    SDL_GPUTextureCreateInfo colorInfo{};
    colorInfo.type          = SDL_GPU_TEXTURETYPE_2D;
    colorInfo.format        = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM; // typical HDR: R16G16B16A16_FLOAT
    colorInfo.width         = static_cast<Uint32>(mWidth);
    colorInfo.height        = static_cast<Uint32>(mHeight);
    colorInfo.layer_count_or_depth = 1;
    colorInfo.num_levels    = 1;
    colorInfo.usage         = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET |
                              SDL_GPU_TEXTUREUSAGE_SAMPLER;  // Sampler so you can read this in later passes

    mColorTexture = SDL_CreateGPUTexture(mDevice, &colorInfo);
    if (!mColorTexture) {
        SDL_Log("Failed to create off-screen color target: %s", SDL_GetError());
        return;
    }

    // --- Depth target texture ---
    SDL_GPUTextureCreateInfo depthInfo{};
    depthInfo.type          = SDL_GPU_TEXTURETYPE_2D;
    depthInfo.format        = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    depthInfo.width         = static_cast<Uint32>(mWidth);
    depthInfo.height        = static_cast<Uint32>(mHeight);
    depthInfo.layer_count_or_depth = 1;
    depthInfo.num_levels    = 1;
    depthInfo.usage         = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;

    mDepthTexture = SDL_CreateGPUTexture(mDevice, &depthInfo);
    if (!mDepthTexture) {
        SDL_Log("Failed to create off-screen depth target: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(mDevice, mColorTexture);
        mColorTexture = nullptr;
        return;
    }
}

void RenderTarget::destroyTextures() {
    if (mColorTexture) {
        SDL_ReleaseGPUTexture(mDevice, mColorTexture);
        mColorTexture = nullptr;
    }
    if (mDepthTexture) {
        SDL_ReleaseGPUTexture(mDevice, mDepthTexture);
        mDepthTexture = nullptr;
    }
}

void RenderTarget::getTargetInfo(SDL_GPUColorTargetInfo& colorInfo,
                                 SDL_GPUDepthStencilTargetInfo& depthInfo,
                                 float clearColorR, float clearColorG,
                                 float clearColorB, float clearColorA) const {
    std::memset(&colorInfo, 0, sizeof(colorInfo));
    colorInfo.texture    = mColorTexture;
    colorInfo.clear_color = {clearColorR, clearColorG, clearColorB, clearColorA};
    colorInfo.load_op    = SDL_GPU_LOADOP_CLEAR;
    colorInfo.store_op   = SDL_GPU_STOREOP_STORE;

    std::memset(&depthInfo, 0, sizeof(depthInfo));
    depthInfo.texture           = mDepthTexture;
    depthInfo.clear_depth       = 1.0f;
    depthInfo.load_op           = SDL_GPU_LOADOP_CLEAR;
    depthInfo.store_op          = SDL_GPU_STOREOP_DONT_CARE;
    depthInfo.stencil_load_op   = SDL_GPU_LOADOP_DONT_CARE;
    depthInfo.stencil_store_op  = SDL_GPU_STOREOP_DONT_CARE;
}

bool RenderTarget::resize(int newWidth, int newHeight) {
    if (newWidth == mWidth && newHeight == mHeight)
        return true;
    destroyTextures();
    mWidth  = newWidth;
    mHeight = newHeight;
    createTextures();
    return mColorTexture && mDepthTexture;
}
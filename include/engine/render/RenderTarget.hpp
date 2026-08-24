#ifndef RENDERTARGET_H
#define RENDERTARGET_H
#include <SDL3/SDL_gpu.h>

class RenderTarget {
public:
    /// Create an off-screen render target of the given dimensions
    RenderTarget(SDL_GPUDevice* device, int width, int height);
    ~RenderTarget();

    // No copy, only move
    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;
    RenderTarget(RenderTarget&&) noexcept;
    RenderTarget& operator=(RenderTarget&&) noexcept;

    /// Getters for use when setting up a render pass
    SDL_GPUTexture* getColorTexture() const { return mColorTexture; }
    SDL_GPUTexture* getDepthTexture() const { return mDepthTexture; }
    
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    /// Convenience: fills out a color target info and depth target info.
    /// You then pass these to SDL_BeginGPURenderPass.
    void getTargetInfo(SDL_GPUColorTargetInfo& colorInfo,
                       SDL_GPUDepthStencilTargetInfo& depthInfo,
                       float clearColorR = 0.1f,
                       float clearColorG = 0.1f,
                       float clearColorB = 0.2f,
                       float clearColorA = 1.0f) const;

    /// Resize (re-creates textures)
    bool resize(int newWidth, int newHeight);

private:
    void createTextures();
    void destroyTextures();

    SDL_GPUDevice* mDevice = nullptr;
    SDL_GPUTexture* mColorTexture = nullptr;
    SDL_GPUTexture* mDepthTexture = nullptr;
    int mWidth = 0;
    int mHeight = 0;
};

#endif
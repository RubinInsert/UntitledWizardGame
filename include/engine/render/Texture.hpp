#ifndef TEXTURE_H
#define TEXTURE_H
#include <memory>
#include <SDL3/SDL_gpu.h>
struct GPUTextureDeleter {
    SDL_GPUDevice* device;
    void operator()(SDL_GPUTexture* tex) const {
        if (tex && device) {
            SDL_ReleaseGPUTexture(device, tex);
        }
    }
};
struct Texture {
    /// @brief A non-owning raw pointer to observe the gpu Texture data
    std::unique_ptr<SDL_GPUTexture, GPUTextureDeleter> gpuTexture;
    int width;
    int height;

    Texture(SDL_GPUTexture* tex, int w, int h, SDL_GPUDevice* device)
        : gpuTexture(tex, GPUTextureDeleter{device})
        , width(w)
        , height(h) {}

    SDL_GPUTexture* get() const { return gpuTexture.get(); }
};

#endif
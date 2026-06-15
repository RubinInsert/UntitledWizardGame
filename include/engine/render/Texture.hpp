#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL3/SDL_gpu.h>

struct Texture {
    // 1. Changed to a non-owning raw pointer. 
    // The AssetManager now holds lifetime ownership of the master atlas.
    SDL_GPUTexture* masterAtlas; 

    int width;  // Original pixel width of this asset
    int height; // Original pixel height of this asset

    // 2. Added normalized UV coordinate windows (0.0 to 1.0 scale)
    float uMin;   // Left-side coordinate
    float vMin;   // Top-side coordinate
    float uWidth; // Horizontal texture span width
    float vHeight;// Vertical texture span height

    // Constructor updated to receive atlas-packed parameters
    Texture(SDL_GPUTexture* atlasTex, int w, int h, float u, float v, float tw, float th)
        : masterAtlas(atlasTex)
        , width(w)
        , height(h)
        , uMin(u)
        , vMin(v)
        , uWidth(tw)
        , vHeight(th) {}

    // Keeps your existing getter code completely compatible with the rest of your system
    SDL_GPUTexture* get() const { return masterAtlas; }
};

#endif
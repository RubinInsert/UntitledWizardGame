#include "LTexture.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
LTexture::LTexture():
    mTexture{nullptr},
    mWidth{0},
    mHeight{0}
{

}

LTexture::~LTexture() {
    // Clean up texture
    destroy();
}

bool LTexture::loadFromFile( std::string path) {
    // Clean up texture if it already exists
    destroy();

    // Load surface
    if(SDL_Surface* loadedSurface = IMG_Load( path.c_str()); loadedSurface == nullptr) {
        SDL_Log( "Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError() );
    } else {
        // Create a texture from surface
        if(mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
            SDL_Log( "Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError() );
        } else {
            // Get Image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Clean loaded surface
        SDL_DestroySurface(loadedSurface);
    }
    return mTexture != nullptr;
}

void LTexture::destroy() {
    // Clean up texture;
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::render (float x, float y) {
    SDL_FRect dstRect{x, y, static_cast<float>(mWidth), static_cast<float>(mHeight)};

    // Render texture
    SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}

int LTexture::getHeight() const {
    return mHeight;
}
int LTexture::getWidth() const {
    return mWidth;
}

bool LTexture::isLoaded() {
    return mTexture != nullptr;
}
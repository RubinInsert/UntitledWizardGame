#include "AssetManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

AssetManager::AssetManager(SDL_Renderer* renderer):
    mRenderer{renderer}

{
    
}
AssetManager::~AssetManager() {}
SDL_Texture* AssetManager::getTexture(const std::string& filePath) {
    auto iterator = mTextures.find(filePath);

    if(iterator != mTextures.end()) {
        SDL_Log("Image retrieved from cache!");
        return iterator->second; // If texture already cached; return
    } else {
        std::string absolutePath = std::string(SDL_GetBasePath()) + filePath;
        if(SDL_Surface* loadedSurface = IMG_Load( absolutePath.c_str() ); loadedSurface == nullptr) {
        SDL_Log( "Unable to load image %s! SDL_image error: %s\n", absolutePath.c_str(), SDL_GetError() );
        return nullptr;
        } else {
        // Create a texture from surface
        SDL_Texture* texture = nullptr;
        if(texture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface); texture == nullptr) {
            SDL_Log( "Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError() );
        } else {
            mTextures.emplace(filePath, texture);
        }
        // Clean loaded surface
        SDL_DestroySurface(loadedSurface);
        return texture;
        }
    }
}

void AssetManager::setRenderer(SDL_Renderer* renderer) {
    mRenderer = renderer;
}
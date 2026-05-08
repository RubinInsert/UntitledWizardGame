#include "engine/core/AssetManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

AssetManager::AssetManager(SDL_Renderer* renderer):
    mRenderer{renderer}

{
    
}
AssetManager::~AssetManager() {}
SDL_Texture* AssetManager::getTexture(const std::string& filePath) const {
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
SpriteSheet* AssetManager::getSpriteSheet(const std::string& assetPath, 
                                          float frameWidth, float frameHeight,
                                          int cols, int rows, float marginX, float marginY, float spacingX, float spacingY) {
    auto it = mSpriteSheets.find(assetPath);
    if (it != mSpriteSheets.end()) return &it->second;  // Cached
    
    // Create new sheet (which internally calls getTexture for the asset)
    auto result = mSpriteSheets.emplace(assetPath, SpriteSheet(*this, assetPath, frameWidth, frameHeight, cols, rows, marginX, marginY, spacingX, spacingY));
    return &result.first->second; // Return sprite sheet from pair<std::string, SpriteSheet*>
}

void AssetManager::setRenderer(SDL_Renderer* renderer) {
    mRenderer = renderer;
}
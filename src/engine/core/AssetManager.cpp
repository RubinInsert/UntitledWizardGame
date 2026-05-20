#include "engine/core/AssetManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <stdexcept>
AssetManager::AssetManager(SDL_Renderer* renderer):
    mRenderer{renderer}

{
    
}
AssetManager::~AssetManager() {}
SDL_Texture* AssetManager::getTexture(const std::string& filePath) const {
    auto iterator = mTextures.find(filePath);

    if(iterator != mTextures.end()) {
        SDL_Log("Image retrieved from cache!");
        
        return iterator->second.get(); // If texture already cached; return raw pointer for observation
    } else {
        std::string absolutePath = std::string(SDL_GetBasePath()) + filePath; // SDL_GetBasePath() is cached internally in SDL3. No Freeing is required.
        SDL_Texture* rawTex = IMG_LoadTexture(mRenderer, absolutePath.c_str());
        if(!rawTex) { // If a texture was not found:
            // Return a pre-loaded fallback texture instead of crashing
            if(filePath == "assets/textures/missing_error.png") throw std::runtime_error(filePath + "is missing! No Fallback."); // Prevent infinite recurrsion.
            return getTexture("assets/textures/missing_error.png"); // If a texture was not found, utilise default missing error texture.
        }
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(
        IMG_LoadTexture(mRenderer, absolutePath.c_str()), 
        SDL_DestroyTexture
        ); // Declare with SDL_DestroyTexture as a custom destructor.
        auto cachedTexture = mTextures.emplace(filePath, std::move(texture)); // Transfer ownership of the pointer into the map
        return cachedTexture.first->second.get(); // Return raw pointer for observation
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
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
    SDL_Log("Retrieving:", filePath);
    if(iterator != mTextures.end()) {
        SDL_Log("Image retrieved from cache!");
        
        return iterator->second.get(); // If texture already cached; return raw pointer for observation
    } else {
        std::string absolutePath = std::string(SDL_GetBasePath()) + filePath; // SDL_GetBasePath() is cached internally in SDL3. No Freeing is required.
        SDL_Texture* rawTex = IMG_LoadTexture(mRenderer, absolutePath.c_str());
        SDL_SetTextureScaleMode(rawTex, SDL_SCALEMODE_NEAREST);
        if(!rawTex) { // If a texture was not found:
            // Return a pre-loaded fallback texture instead of crashing
            if(filePath == "assets/textures/missing_error.png") throw std::runtime_error(filePath + "is missing! No Fallback."); // Prevent infinite recurrsion.
            return getTexture("assets/textures/missing_error.png"); // If a texture was not found, utilise default missing error texture.
        }
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(
        rawTex, 
        SDL_DestroyTexture
        ); // Declare with SDL_DestroyTexture as a custom destructor.
        auto cachedTexture = mTextures.emplace(filePath, std::move(texture)); // Transfer ownership of the pointer into the map
        return cachedTexture.first->second.get(); // Return raw pointer for observation
    }
}
SpriteSheet* AssetManager::getSpriteSheet(const std::string& filePath, 
                                          float frameWidth, float frameHeight,
                                          int cols, int rows, float marginX, float marginY, float spacingX, float spacingY) {
    auto it = mSpriteSheets.find(filePath);
    if (it != mSpriteSheets.end()) {
        return it->second.get();  // Return raw observer pointer
    }
    
    // Create dynamically on the heap inside a unique_ptr
    auto spriteSheet = std::make_unique<SpriteSheet>(*this, filePath, frameWidth, frameHeight, cols, rows, marginX, marginY, spacingX, spacingY);
    
    auto cachedSpriteSheet = mSpriteSheets.emplace(filePath, std::move(spriteSheet));
    return cachedSpriteSheet.first->second.get(); // Return observable raw pointer
}

void AssetManager::setRenderer(SDL_Renderer* renderer) {
    mRenderer = renderer;
}
#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <SDL3/SDL.H>
#include <string>
#include <unordered_map>
#include "engine/render/SpriteSheet.hpp"
class AssetManager {
public:
    AssetManager(SDL_Renderer* renderer);
    ~AssetManager();

    SDL_Texture* getTexture(const std::string& filePath) const;
    SpriteSheet* getSpriteSheet(const std::string& name, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY);

    void setRenderer(SDL_Renderer* renderer);
    void Clear();

private:
    SDL_Renderer* mRenderer;
    // Library of loaded textures (for repetition of use)
    mutable std::unordered_map<std::string, SDL_Texture*> mTextures;
    // Same library concept but for sprite sheets
    mutable std::unordered_map<std::string, SpriteSheet> mSpriteSheets;
};
#endif
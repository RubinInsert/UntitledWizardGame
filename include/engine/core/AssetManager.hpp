#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <SDL3/SDL.H>
#include <string>
#include <unordered_map>
#include <memory>
#include "engine/render/SpriteSheet.hpp"
class AssetManager {
public:
    AssetManager(SDL_Renderer* renderer);
    ~AssetManager();

    /// @brief Retrieves an SDL_Texture from the cache, loading it from the disk if not present in cache.
    /// @param filePath The relative path to the asset (e.g., "assets/textures/hero.png")
    /// @return A non-owning, observe-only pointer to the asset's SDL_Texture
    /// @note Handled safely via internal fallback texture if the target file is missing.
    /// @throws std::runtime_error if both the target and the fallback asset fail to load.
    // If loading fails, logs an error and attempts to return a fallback missing-texture asset
    // Throws std::runtime_error if the fallback asset itself cannot be loaded.
    SDL_Texture* getTexture(const std::string& filePath) const;
    SpriteSheet* getSpriteSheet(const std::string& name, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY);

    void setRenderer(SDL_Renderer* renderer);
    void Clear();

private:
    SDL_Renderer* mRenderer;
    // Library of loaded textures owned solely by the AssetManager
    mutable std::unordered_map<std::string, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> mTextures;
    // Same library concept but for sprite sheets
    mutable std::unordered_map<std::string, SpriteSheet> mSpriteSheets;
};
#endif
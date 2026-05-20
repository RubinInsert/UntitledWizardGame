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
    SDL_Texture* getTexture(const std::string& filePath) const;
    /// @brief Retrieves a SpriteSheet from the cache, loading it from the disk if not present in cache.
    /// @param filePath The relative path to the asset (e.g., "assets/textures/hero.png")
    /// @param frameWidth The pixel width of each individual frame in the sprite sheet
    /// @param frameHeight The pixel height of each individual frame in the sprite sheet
    /// @param cols The number of columns in the spritesheet
    /// @param rows The number of rows in the sprite sheet
    /// @param marginX The distance from the SpriteSheet left/right edge to the first frame
    /// @param marginY The distance from the SpriteSheet top/bottom edge to the first frame
    /// @param spacingX The distance between two frames on the X axis
    /// @param spacingY The distance between two frames on the Y axis
    /// @return A non-owning, observe-only pointer to the asset's SpriteSheet
    SpriteSheet* getSpriteSheet(const std::string& filePath, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY);

    void setRenderer(SDL_Renderer* renderer);
    void Clear();

private:
    SDL_Renderer* mRenderer;
    // Library of loaded textures owned solely by the AssetManager
    mutable std::unordered_map<std::string, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> mTextures;
    // Library of loaded SpriteSheet owned solely by the AssetManager. Utilizes unique pointer to avoid pointer invalidation in unordered map
    mutable std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> mSpriteSheets;
};
#endif
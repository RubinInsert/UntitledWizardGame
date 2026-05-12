#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <SDL3/SDL.H>
#include <string>
#include <unordered_map>
#include "engine/render/SpriteSheet.hpp"
#include "engine/core/Audio.hpp"
class AssetManager {
public:
    AssetManager(SDL_Renderer* renderer);
    ~AssetManager();

    SDL_Texture* getTexture(const std::string& filePath) const;
    SpriteSheet* getSpriteSheet(const std::string& name, float frameWidth, float frameHeight, int cols, int rows, float marginX, float marginY, float spacingX, float spacingY);
    bool getAudioStream(const std::string& filePath, Audio &outAudio) const;
    void setRenderer(SDL_Renderer* renderer);
    void Clear();

private:
    SDL_Renderer* mRenderer;
    // Library of loaded textures (for repetition of use)
    mutable std::unordered_map<std::string, SDL_Texture*> mTextures;
    // Same library concept but for sprite sheets
    mutable std::unordered_map<std::string, SpriteSheet> mSpriteSheets;
    mutable std::unordered_map<std::string, Audio> mAudioStreams;
};
#endif
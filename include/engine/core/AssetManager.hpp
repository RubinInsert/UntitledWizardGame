#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <SDL3/SDL.H>
#include <string>
#include <unordered_map>
class AssetManager {
public:
    AssetManager(SDL_Renderer* renderer);
    ~AssetManager();

    SDL_Texture* getTexture(const std::string& filePath);

    void setRenderer(SDL_Renderer* renderer);
    void Clear();

private:
    SDL_Renderer* mRenderer;

    // Library of loaded textures (for repetition of use)
    std::unordered_map<std::string, SDL_Texture*> mTextures;
};
#endif
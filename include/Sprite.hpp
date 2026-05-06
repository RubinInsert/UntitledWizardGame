#ifndef SPRITE_H
#define SPRITE_H
#include <SDL3/SDL.h>
#include <string>
#include "AssetManager.hpp"
class Sprite {
    public:
    Sprite(std::string texturePath, AssetManager& assetManager, float x, float y);
    ~Sprite();
    void destroy();
    SDL_Texture* mTexture;
    SDL_FRect srcRect;
    SDL_FRect destRect;

};
#endif
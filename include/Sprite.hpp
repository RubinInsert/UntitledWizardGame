#ifndef SPRITE_H
#define SPRITE_H
#include <SDL3/SDL.h>
#include <string>
#include "AssetManager.hpp"
struct Sprite {
    SDL_Texture* texture{nullptr};
    SDL_FRect srcRect;
};
#endif
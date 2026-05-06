#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <SDL3/SDL.h>
#include <string>
#include "AssetManager.hpp"
struct Transform {
    SDL_FPoint position{};
    SDL_FPoint size{};
};
#endif
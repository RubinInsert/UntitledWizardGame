#ifndef TILE_H
#define TILE_H
#include <string>
#include <sdl3/SDL.h>
struct TileTexture {
/// @brief A non-owning pointer to the SDL texture
SDL_Texture* srcImage;
SDL_FRect srcRect;
bool isObject;
};

#endif
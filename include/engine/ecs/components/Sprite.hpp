#ifndef SPRITE_H
#define SPRITE_H
#include <SDL3/SDL.h>
#include <string>

class SpriteSheet;  // Forward declaration

struct Sprite {
    SpriteSheet* src;
    int frame;
};
#endif
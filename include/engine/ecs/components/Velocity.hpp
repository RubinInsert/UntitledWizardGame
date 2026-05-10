#ifndef VELOCITY_H
#define VELOCITY_H
#include <SDL3/SDL.h>
#include <string>
enum Direction {
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST,
    NONE
};
struct Velocity {
    SDL_FPoint velocity{};
    Direction direction;
};
#endif
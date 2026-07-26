#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include <SDL3/SDL.h>
#include "engine/core/WorldSettings.hpp"
#include "engine/render/Camera.hpp"
#include <cmath>
class Coordinate {
public:
    // Converts "top-down" World coordinates to ISOmetric coordinates before any camera offets
    static SDL_FPoint WorldToIso(float wx, float wy, const WorldSettings& settings) {
    // 1. Convert Cartesian (+Y Up) to Grid (+Y Down)
    // We flip wy here so that a high Y value in World Space 
    // results in a "higher" (lower value) position on the grid.
    float gridX = wx;
    float gridY = -wy; 

    // 2. Project to Isometric Space
    float isoX = (gridX - gridY) * (settings.tileWidth * 0.5f);
    float isoY = (gridX + gridY) * (settings.tileHeight * 0.5f);
    
    return { isoX, isoY };
}

    // Applies Camera offsets to Isometric projection to give (X, Y) in pixel space.
    static SDL_FPoint IsoToScreen(float isoX, float isoY, const Camera& camera) {
        return { 0, 0 };
    }
    // Converts Grid space (int x, int y) to the pixel coordinate on the screen
    // Differs from WorldToScreen() by not applying a flip on the Y axis: (X+, Y-) 
    static SDL_FPoint GridToScreen(int gx, int gy, const Camera& camera, const WorldSettings& settings) {
    // Treat the integer indices as the center of the tile by adding 0.5f
    float x = static_cast<float>(gx) + 0.5f;
    float y = static_cast<float>(gy) + 0.5f;

    float isoX = (x - y) * (settings.tileWidth * 0.5f);
    float isoY = (x + y) * (settings.tileHeight * 0.5f);
    
    SDL_FPoint screen = IsoToScreen(isoX, isoY, camera);
    
    return { screen.x, screen.y };
}
struct GridPoint { int x; int y; };

// Converts a world point to grid coordinates by flooring and flipping Y axis.
// Used for calling getTile() from a world coordinate
static GridPoint WorldToGrid(float wx, float wy) {
    // 1. Handle the Cartesian (+Y Up) to Grid (+Y Down) flip
    // 2. Use floor to ensure -0.1 becomes -1, correctly identifying tiles
    return {
        static_cast<int>(std::floor(wx)),
        static_cast<int>(std::floor(-wy))
    };
}
    // Converts from World point to Screen pixel space
    static SDL_FPoint WorldToScreen(float wx, float wy, const Camera& camera, const WorldSettings& settings) {
        auto iso = WorldToIso(wx, wy, settings);
        return IsoToScreen(iso.x, iso.y, camera);
    }

    // Converts from Screen Pixel Space to World point
    static SDL_FPoint ScreenToWorld(float sx, float sy, const Camera& camera, const WorldSettings& settings) {
        return { 0, 0 };
    }

        // Converts raw pixel values from a Tiled Isometric Map into your top-down World Coordinates (+Y Up)
    static SDL_FPoint TiledIsoObjectToWorld(float tiledX, float tiledY) {
        float tileX = tiledX / WorldSettings::tileHeight;
        float tileY = tiledY / WorldSettings::tileHeight;
        float wx = tileX;
        float wy = -tileY;
        return { wx, wy };
    }
};

#endif
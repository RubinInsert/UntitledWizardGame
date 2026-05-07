#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <SDL3/SDL.h>
#include "engine/render/Camera.hpp"
#include "engine/core/WorldSettings.hpp"

class TileMap {
public:
    TileMap(int width, int height, SDL_Texture* tileTexture);
    ~TileMap();

    void render(SDL_Renderer& renderer, const Camera& camera, const WorldSettings& worldSettings);
    
    int getTile(int x, int y) const;
    void setTile(int x, int y, int tileID);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int width;
    int height;
    SDL_Texture* tileTexture;
    std::vector<int> tileData;
};

#endif
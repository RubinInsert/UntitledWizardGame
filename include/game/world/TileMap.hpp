#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <SDL3/SDL.h>
#include "engine/render/Camera.hpp"
#include "engine/core/WorldSettings.hpp"
#include "engine/render/SpriteSheet.hpp"
class TileMap {
public:
    TileMap(int width, int height, SpriteSheet* spriteSheet);
    ~TileMap();

    void render(SDL_Renderer& renderer, const Camera& camera, const WorldSettings& worldSettings);
    
    int getTile(int x, int y) const;
    void setTile(int x, int y, int tileID);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setTileData(const std::vector<int>& data);
private:
    int width;
    int height;
    SpriteSheet* spriteSheet;
    std::vector<int> tileData;
};

#endif
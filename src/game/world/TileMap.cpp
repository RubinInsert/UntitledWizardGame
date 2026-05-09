#include "game/world/TileMap.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/render/Coordinate.hpp"
#include <SDL3/SDL.h>

TileMap::TileMap(int width, int height, SpriteSheet* spriteSheet)
    : width(width), height(height), spriteSheet(spriteSheet) {
    tileData.resize(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tileData[y * width + x] = (x + y) % 10;
        }
    }
    tileData.resize(width * height, 0);  // All tiles are ID 0
}

TileMap::~TileMap() {
    // Don't delete tileTexture; Game owns it
}

int TileMap::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return -1;
    return tileData[y * width + x];
}

void TileMap::setTile(int x, int y, int tileID) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        tileData[y * width + x] = tileID;
    }
}
void TileMap::setTileData(const std::vector<int>& data) { tileData = data; }
void TileMap::render(SDL_Renderer& renderer, const Camera& camera, const WorldSettings& worldSettings) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tileID = getTile(x, y); // Get the tile at (x, y) world coordinates
            if (tileID == -1) continue;

            // 1. Convert Grid coordinates to screen Screen (No Y-flip needed, y is already Grid Space)
            SDL_FPoint screen = Coordinate::GridToScreen(x, y, camera, worldSettings);
            // Scale width/height according to camera zoom;
            float finalW = worldSettings.tileWidth * camera.zoom;
            float finalH = worldSettings.tileHeight * camera.zoom;

            // 4. DRAW CENTERING
            SDL_FRect destRect {
                screen.x - (finalW * 0.5f),
                screen.y - (finalH * 0.5f),
                finalW,
                finalH
            };

            SDL_RenderTexture(&renderer, spriteSheet->getTexture(), &spriteSheet->getFrame(tileID), &destRect);
        }
    }
}
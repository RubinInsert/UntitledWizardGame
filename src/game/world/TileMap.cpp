#include "game/world/TileMap.hpp"
#include <SDL3/SDL.h>

TileMap::TileMap(int width, int height, SDL_Texture* tileTexture)
    : width(width), height(height), tileTexture(tileTexture) {
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

void TileMap::render(SDL_Renderer& renderer, const Camera& camera, const WorldSettings& worldSettings) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Get the tile type
            int tileID = getTile(x, y);
            // Exception ID for empty tile
            if (tileID == -1) continue;  // Skip invalid tiles
            // Isometric projection
            float isoX = (x - y) * (worldSettings.tileWidth * 0.5f);
            float isoY = (x + y) * (worldSettings.tileHeight * 0.5f);

            // Camera offset and viewport centering
            float screenX = (isoX - camera.position.x) * camera.zoom + camera.viewportWidth * 0.5f;
            float screenY = (isoY - camera.position.y) * camera.zoom + camera.viewportHeight * 0.5f;

            // Draw tile
            SDL_FRect destRect{
                screenX,
                screenY,
                worldSettings.tileWidth * camera.zoom,
                worldSettings.tileHeight * camera.zoom
            };

            SDL_RenderTexture(&renderer, tileTexture, nullptr, &destRect);
        }
    }
}
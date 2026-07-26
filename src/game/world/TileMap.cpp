#include "game/world/TileMap.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/render/Coordinate.hpp"
#include <SDL3/SDL.h>

TileMap::TileMap(int width, int height)
    : width(width), height(height) {
    tileData.resize(width * height, 0);  // Initialize all tiles as empty (ID 0)
}
TileMap::TileMap()
    : width(0), height(0) {}
TileMap::~TileMap() {
    // Don't delete tileTexture; Game owns it
}

int TileMap::getTile(int x, int y, const TileLayer& layer) const {
    if (x < 0 || x >= layer.width || y < 0 || y >= layer.height) return -1;
    return layer.tileData[y * layer.width + x];
}

void TileMap::setTile(int x, int y, int tileID) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        tileData[y * width + x] = tileID;
    }
}
void TileMap::render(RenderSystem& renderSystem, const Camera& camera, const WorldSettings& worldSettings) {
    // TODO: Reimplement for 3D rendering pipeline
    // for(TileLayer& layer : layers) {
    //     if(layer.isVisible == false) return;
    //     for (int y = 0; y < height; ++y) {
    //         for (int x = 0; x < width; ++x) {
    //             int tileID = getTile(x, y, layer);
    //             if (tileID == -1) continue;
    //
    //             auto it = globalIdToSprite.find(tileID);
    //             if (it == globalIdToSprite.end()) continue;
    //
    //             const Sprite& sprite = it->second;
    //             SpriteSheet* sheet = sprite.src;
    //             if (!sheet) continue;
    //             SDL_FRect tileDimensions = sheet->getFrame(sprite.frame);
    //             SDL_FPoint screen = Coordinate::GridToScreen(x, y, camera, worldSettings);
    //             float finalW = tileDimensions.w;
    //             float finalH = tileDimensions.h;
    //             SDL_FRect destRect {
    //                 screen.x - (finalW * 0.5f),
    //                 screen.y - (finalH) + (0.5f * WorldSettings::tileHeight),
    //                 finalW,
    //                 finalH
    //             };
    //             // 2D sprite draw call removed — adapting to 3D
    //         }
    //     }
    // }
}

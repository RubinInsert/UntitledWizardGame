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
    for(TileLayer& layer : layers) {
        if(layer.isVisible == false) return;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int tileID = getTile(x, y, layer); // Get the tile at (x, y) world coordinates
                if (tileID == -1) continue;
                
                auto it = globalIdToSprite.find(tileID);
                if (it == globalIdToSprite.end()) {
                    continue; // Sprite asset not found for this ID, skip drawing it
                }
                const Sprite& sprite = it->second;
                SpriteSheet* sheet = sprite.src;
                if (!sheet) continue;
                SDL_FRect tileDimensions = sheet->getFrame(sprite.frame);
                // 1. Convert Grid coordinates to screen Screen (No Y-flip needed, y is already Grid Space)
                SDL_FPoint screen = Coordinate::GridToScreen(x, y, camera, worldSettings);
                // Scale width/height according to camera zoom;
                float finalW = tileDimensions.w * camera.zoom;
                float finalH = tileDimensions.h * camera.zoom;

                // 4. DRAW CENTERING
                SDL_FRect destRect { // Bottom-Center Anchor point
                    screen.x - (finalW * 0.5f),
                    screen.y - (finalH) + (0.5f * WorldSettings::tileHeight * camera.zoom), // TileHeight is in map units, so it must be multiplied by zoom
                    finalW,
                    finalH
                };
                RenderSprite renderSprite{
                    sprite.src->getTexture(),
                    sheet->getFrame(sprite.frame),
                    destRect,
                    0, // Layer
                    SDL_Color{255, 255, 255, 255} // Color modification
                };
                renderSystem.draw(renderSprite);
                // SDL_RenderTexture(
                //     &renderer, 
                //     sheet->getTexture(), 
                //     &sheet->getFrame(sprite.frame),
                //     &destRect
                // );
            }
        }
    } 
}
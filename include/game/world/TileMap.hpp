#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <unordered_map>
#include <SDL3/SDL.h>
#include "engine/render/Camera.hpp"
#include "engine/core/WorldSettings.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "game/world/TileLayer.hpp"
class TileMap {
public:
    TileMap(int width, int height);
    TileMap();
    ~TileMap();

    void render(SDL_Renderer& renderer, const Camera& camera, const WorldSettings& worldSettings);
    
    int getTile(int x, int y, const TileLayer& layer) const;
    void setTile(int x, int y, int tileID);
    
    // Read-only access (for rendering/logic)
    const std::vector<TileLayer>& getLayers() const { return layers; }

    // Mutable access (for the MapLoader or editors)
    std::vector<TileLayer>& getLayers() { return layers; }

    // Allow the MapLoader to add layers during construction
    TileLayer& addLayer(TileLayer layer) {
        layers.push_back(std::move(layer));
        return layers.back();
    }
    int getWidth() const { return width; }
    void setWidth(int w) {width = w;}
    int getHeight() const { return height; }
    void setHeight(int h) {height = h;}
    void setCollisionLayerIndex(int layerIndex) { collisionLayerIndex = layerIndex; }
    const TileLayer* getCollisionLayer() const {
    if (collisionLayerIndex < 0 || collisionLayerIndex >= static_cast<int>(layers.size())) {
        return nullptr;
    }
    return &layers[collisionLayerIndex];
    }

    void setGidAssetMap(const std::unordered_map<int, Sprite> assetMap) {
        globalIdToSprite = std::move(assetMap);
    }
private:
    int width;
    int height;
    std::vector<TileLayer> layers;
    int collisionLayerIndex = -1;
    std::unordered_map<int, Sprite> globalIdToSprite;
    std::vector<int> tileData;
};

#endif
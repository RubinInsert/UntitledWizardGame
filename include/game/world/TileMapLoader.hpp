#ifndef TILEMAPLOADER_H
#define TILEMAPLOADER_H
#include <string>
#include "game/world/TileMap.hpp"
#include "game/world/ObjectLoader.hpp"
#include <entt/entt.hpp>
#include "engine/core/AssetManager.hpp"
struct LoadedTileSet {
    int columns;
    int rows;
    int margin;
    int spacing;
};
struct LoadedTileLayer {
    std::string name;
    int width;
    int height;
    std::vector<int> tileIds;
    std::string tilesetImagePath;
    int tilesetFirstGid;
    LoadedTileSet tileSet;
};

struct LoadedMap {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    int collisionLayerIndex;
    std::vector<LoadedTileLayer> layers;
    std::vector<entt::entity> objects;
};


class TileMapLoader {
public:
    static LoadedMap loadTileMap(const json& mapJSON, entt::registry& registry, AssetManager& assetManager, Camera& camera, WorldSettings worldSettings);
};

#endif
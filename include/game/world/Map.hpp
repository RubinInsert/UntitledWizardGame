#ifndef MAP_H
#define MAP_H
#include <vector>
#include <entt/entt.hpp>
#include "game/world/TileMap.hpp"
struct Map {
    /// @brief Represents the layers of stationary isometric tiles to be rendered 
    TileMap tileMap;
    /// @brief Represents the free-form objects that are larger than a single tile (e.g. buildings)
    std::vector<entt::entity> objects;
    /// @brief The layer that is used for tile-based AABB Collisions. The layer indicated here will be skipped in rendering.
    int collisionLayerIndex;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};
#endif
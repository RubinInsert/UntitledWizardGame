#include "game/movement/CollisionSystem.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "engine/render/DebugDrawer.hpp"
void CollisionSystem::update(entt::registry& registry, const TileMap& tileMap) {
    // Only check collision for entities that have velocity (moving entities)
    auto view = registry.view<Transform, Velocity>();
    for(auto& entity : view) {
        auto& transform = view.get<Transform>(entity);


        // `transform.position` is in world grid units (+Y up).
        // Compute world-space bbox (feet anchor at transform.position).
        float wx = transform.position.x;
        float wy = transform.position.y;
        float left = wx - (transform.size.x * 0.5f);
        float right = wx + (transform.size.x * 0.5f);
        float bottom = wy;                    // feet
        float top = wy + transform.size.y;    // top of sprite (higher in world Y)
        // Convert world y (up-positive) to grid y (down-positive) for tile lookups:
        auto minGrid = Coordinate::WorldToGrid(left, top);     // Top-Left corner
        auto maxGrid = Coordinate::WorldToGrid(right, bottom); // Bottom-Right corner
        int minTileX = minGrid.x;
        int maxTileX = static_cast<int>(std::floor(right - 0.0001f)); // Keep the epsilon buffer for X
        int minTileY = minGrid.y;
        int maxTileY = maxGrid.y;

        // Draw debug box in world coords (center + positive width/height)
        float dbgW = right - left;
        float dbgH = top - bottom;
        float dbgCX = left + dbgW * 0.5f;
        float dbgCY = bottom + dbgH * 0.5f;
        float entityCenterY = transform.position.y + (transform.size.y * 0.5f);
        DebugDrawer::AddBox(transform.position.x, entityCenterY, transform.size.x, transform.size.y);

        // Check all tiles the sprite overlaps (tileMap expects grid coords y-down)
        // Only tiles with ID > 0 are solid; ID 0 is empty
        bool collision = false;
        TileLayer* collisionLayer = tileMap.getCollisionLayer();
        if(collisionLayer == nullptr) return;
        for (int x = minTileX; x <= maxTileX; ++x) {
            for (int y = minTileY; y <= maxTileY; ++y) {
                if (tileMap.getTile(x, y, *collisionLayer) != -1) {
                    // Use the negative Y for the DebugDrawer since it flips it back to positive
                    DebugDrawer::AddBox(static_cast<float>(x) + 0.5f, -(static_cast<float>(y) + 0.5f), 1.f, 1.f);
                    collision = true;
                    break;
                }
            }
            if (collision) break;
        }
        // Debug: Show all tile collision boxes in 50x50 area
        // for(int x = 0; x <= 50; ++x) {
        //     for(int y = 0; y <= 50; ++y) {
        //         if(tileMap.getTile(x, y) != -1) {
        //             DebugDrawer::AddBox(static_cast<float>(x) + 0.5f, -(static_cast<float>(y) + 0.5f), 1.f, 1.f);
        //         }
        //     }
        // }
        if (collision) {
            transform.position = transform.previousPosition;
        }
    }
}
#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include <entt/entt.hpp>
#include "game/world/TileMap.hpp"
class CollisionSystem {
    public:
        void update(entt::registry& registry, const TileMap& tileMap);
};

#endif
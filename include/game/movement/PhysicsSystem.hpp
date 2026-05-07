#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include <entt/entt.hpp>

class PhysicsSystem {
    public:
    void update(entt::registry& registry, double deltaTime);
};

#endif
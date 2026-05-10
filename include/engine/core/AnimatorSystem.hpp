#ifndef ANIMATORSYSTEM_H
#define ANIMATORSYSTEM_H
#include <entt/entt.hpp>
class AnimatorSystem {
    public:
    void update(entt::registry& registry, float deltaTime);
};

#endif
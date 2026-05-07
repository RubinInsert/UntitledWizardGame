#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H
#include <entt/entt.hpp>
#include "engine/core/InputManager.hpp"
class PlayerController {
    public:
    void update(entt::registry& registry, entt::entity player, const InputManager& input, double deltaTime);
};

#endif
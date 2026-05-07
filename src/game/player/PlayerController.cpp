#include "game/player/PlayerController.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
void PlayerController::update(entt::registry& registry, entt::entity player, const InputManager& input, double deltaTime) {
    if(player == entt::null || !registry.valid(player)) return;
    Transform& transform = registry.get<Transform>(player);
    Velocity& velocity = registry.get<Velocity>(player);
    MovementStats& movementStats = registry.get<MovementStats>(player);
    velocity.velocity.x = 0;
    velocity.velocity.y = 0;
    if(input.isKeyDown(SDLK_A)) {
        velocity.velocity.x -= movementStats.walkSpeed;
    }
    if(input.isKeyDown(SDLK_D)) {
        velocity.velocity.x += movementStats.walkSpeed;
    }
    if(input.isKeyDown(SDLK_W)) {
        velocity.velocity.y -= movementStats.walkSpeed;
    }
    if(input.isKeyDown(SDLK_S)) {
        velocity.velocity.y += movementStats.walkSpeed;
    }
}
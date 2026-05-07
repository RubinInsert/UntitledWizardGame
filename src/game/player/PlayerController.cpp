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
    if(input.isKeyDown(SDLK_A)) velocity.velocity.x -= 1.0f;
    if(input.isKeyDown(SDLK_D)) velocity.velocity.x += 1.0f;
    if(input.isKeyDown(SDLK_W)) velocity.velocity.y -= 1.0f;
    if(input.isKeyDown(SDLK_S)) velocity.velocity.y += 1.0f;

    // Normalize movement so diagonals arent faster
    float lengthSq = velocity.velocity.x * velocity.velocity.x
                    + velocity.velocity.y * velocity.velocity.y;

    if(lengthSq > 0.0f) {
        float invLength = 1.0f / std::sqrt(lengthSq);
        velocity.velocity.x *= invLength;
        velocity.velocity.y *= invLength;
    }
    // Scale movement to walking speed
    velocity.velocity.x *= movementStats.walkSpeed;
    velocity.velocity.y *= movementStats.walkSpeed;

}
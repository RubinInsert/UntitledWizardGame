#include "game/player/PlayerController.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
#include "engine/ecs/components/Animation.hpp"
#include "game/player/PlayerAnims.hpp"
#include "engine/ecs/components/Sprite.hpp"
void PlayerController::update(entt::registry& registry, entt::entity player, const InputManager& input, double deltaTime) {
    if(player == entt::null || !registry.valid(player)) return;
    Transform& transform = registry.get<Transform>(player);
    Velocity& velocity = registry.get<Velocity>(player);
    Animation& animation = registry.get<Animation>(player);
    Sprite& sprite = registry.get<Sprite>(player);
    MovementStats& movementStats = registry.get<MovementStats>(player);
    velocity.velocity.x = 0;
    velocity.velocity.y = 0;
    if(input.isKeyDown(SDLK_A)) velocity.velocity.x -= 1.0f;
    if(input.isKeyDown(SDLK_D)) velocity.velocity.x += 1.0f;
    if(input.isKeyDown(SDLK_W)) velocity.velocity.y += 1.0f;
    if(input.isKeyDown(SDLK_S)) velocity.velocity.y -= 1.0f;

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
    AnimationSequence nextSeq = animation.sequence;
    // Handle Animations
    switch(velocity.direction) {
        case Direction::NORTH:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_NORTH : PlayerAnims::IDLE_NORTH;
            break;
        case Direction::NORTHEAST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_NORTHEAST : PlayerAnims::IDLE_NORTHEAST;
            break;
        case Direction::EAST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_EAST : PlayerAnims::IDLE_EAST;
            break;
        case Direction::SOUTHEAST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_SOUTHEAST : PlayerAnims::IDLE_SOUTHEAST;
            break;
        case Direction::SOUTH:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_SOUTH : PlayerAnims::IDLE_SOUTH;
            break;
        case Direction::SOUTHWEST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_SOUTHWEST : PlayerAnims::IDLE_SOUTHWEST;
            break;
        case Direction::WEST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_WEST : PlayerAnims::IDLE_WEST;
            break;
        case Direction::NORTHWEST:
            nextSeq = lengthSq > 0 ? PlayerAnims::WALK_NORTHWEST : PlayerAnims::IDLE_NORTHWEST;
            break;
        case Direction::NONE:
            nextSeq = PlayerAnims::IDLE_EAST;
            break;
    }
    if (nextSeq.name != animation.sequence.name) {
        animation.sequence = nextSeq;
        animation.timer = 0.0f;
        animation.currentFrameOffset = 0;
        
        // Immediate visual sync
        sprite.frame = animation.sequence.startFrame;
    }
}
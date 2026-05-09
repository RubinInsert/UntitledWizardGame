#include "game/movement/PhysicsSystem.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
void PhysicsSystem::update(entt::registry& registry, double deltaTime) {
    auto view = registry.view<Transform, Velocity>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& velocity = view.get<Velocity>(entity);
        transform.previousPosition = transform.position; // Track incase new position defies collision system
        transform.position.x += velocity.velocity.x * static_cast<float>(deltaTime);
        transform.position.y += velocity.velocity.y * static_cast<float>(deltaTime);
    }
}
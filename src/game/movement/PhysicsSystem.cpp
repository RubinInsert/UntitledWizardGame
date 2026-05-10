#include "game/movement/PhysicsSystem.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
namespace { // Anonymous function
    // Solution provided by Eric Bainville (2009),
    // https://stackoverflow.com/questions/1437790/how-to-snap-a-directional-2d-vector-to-a-compass-n-ne-e-se-s-sw-w-nw
    // Avoids expensive atan2 calls.
    Direction compass(double x,double y)
    {
    double t = 0.392699082; // tan(M_PI/8.0);

    if (x>=0)
    {
        if (y>=0)
        {
        if (x>y) { if (y<t*x) return Direction::EAST; }
        else { if (x<t*y) return Direction::NORTH; }
        return Direction::NORTHEAST;
        }
        else
        {
        if (-y<t*x) return Direction::EAST;
        if (x<-t*y) return Direction::SOUTH;
        return Direction::SOUTHEAST;
        }
    }
    else
    {
        if (y>=0)
        {
        if (y<-t*x) return Direction::WEST;
        if (-x<t*y) return Direction::NORTH;
        return Direction::NORTHWEST;
        }
        else
        {
        if (-y<-t*x) return Direction::WEST;
        if (-x<-t*y) return Direction::SOUTH;
        return Direction::SOUTHWEST;
        }
    }
    return Direction::EAST;
    }
}
void PhysicsSystem::update(entt::registry& registry, double deltaTime) {
    auto view = registry.view<Transform, Velocity>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& velocity = view.get<Velocity>(entity);
        transform.previousPosition = transform.position; // Track incase new position defies collision system
        transform.position.x += velocity.velocity.x * static_cast<float>(deltaTime);
        transform.position.y += velocity.velocity.y * static_cast<float>(deltaTime);
        float lengthSq = (velocity.velocity.x * velocity.velocity.x) + (velocity.velocity.y * velocity.velocity.y);
        if(lengthSq > 0.f) { // If velocity has a magnitude
            velocity.direction = compass(velocity.velocity.x, velocity.velocity.y);
        }
    }
}

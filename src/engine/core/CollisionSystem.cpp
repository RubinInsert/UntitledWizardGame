#include "engine/core/CollisionSystem.hpp"
#include "engine/ecs/components/PhysicsComponents.hpp"
#include "engine/ecs/components/Transform.hpp"
void CollisionSystem::Init(entt::registry& registry) {
    worldDef = b3DefaultWorldDef();
    worldDef.gravity = b3Vec3{0.0f, -10.0f, 0.0f};

    worldId = b3CreateWorld(&worldDef);
    
    // Automatically collect entities when CollisionBoxComponent is added
        registry.on_construct<CollisionBoxComponent>().connect<&CollisionSystem::RegisterObjectCollisionBox>(this);
}

void CollisionSystem::Step(entt::registry& registry) {
     b3World_Step(worldId, 1.0f / 60.0f, 4);
    
     // Sync Box3D position & rotation back to EnTT Transform components
    auto view = registry.view<Transform, RigidBodyComponent>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& rb = view.get<RigidBodyComponent>(entity);

        if (b3Body_IsValid(rb.id) && b3Body_GetType(rb.id) == b3_dynamicBody) {
            b3Vec3 pos = b3Body_GetPosition(rb.id);
            b3Quat rot = b3Body_GetRotation(rb.id);

            // Update GLM position
            transform.position = glm::vec3{pos.x, pos.y, pos.z};

            // Update GLM rotation (GLM quat constructor is w, x, y, z -> Box3D holds s, v.x, v.y, v.z)
            transform.rotation = glm::quat{rot.s, rot.v.x, rot.v.y, rot.v.z};
        }
    }
}

void CollisionSystem::RegisterObjectCollisionBox(entt::registry& registry, entt::entity entity) {
    auto& box = registry.get<CollisionBoxComponent>(entity);

    Transform* transform = registry.try_get<Transform>(entity);
    RigidBodyComponent& rb = registry.get_or_emplace<RigidBodyComponent>(entity);

    // Create Box3D default body definition
    b3BodyDef bodyDef = b3DefaultBodyDef();

    if (transform) {
        glm::vec3 pos = transform->position;
        glm::quat rot = transform->rotation;
        bodyDef.position = b3Pos{pos.x, pos.y, pos.z};
        bodyDef.rotation = b3Quat{b3Vec3{rot.x, rot.y, rot.z}, rot.w};
    }

    bodyDef.type = rb.type == BodyType::Dynamic ? b3_dynamicBody : b3_staticBody;


    b3BodyId bodyId = b3CreateBody(worldId, &bodyDef);
    rb.id = bodyId;
    b3BoxHull boxHull = b3MakeBoxHull(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
    b3ShapeDef boxShapeDef = b3DefaultShapeDef();
    if (rb.type == BodyType::Dynamic) {
        boxShapeDef.density = 1.0f; // Dynamic bodies REQUIRE non-zero density
        boxShapeDef.baseMaterial.friction = 0.3f;
    }
    b3CreateHullShape(bodyId, &boxShapeDef, &boxHull.base);
}

// Any object with a CollisionBoxComponent should add a corresponding b3BodyId + b3BodyDef

// Any object with a CollisionBoxComponent AND a RigidBodyComponent should add a corresponding:
// - b3BodyDef
// - b3BodyId
// - b3BoxHull
// - b3ShapeDef
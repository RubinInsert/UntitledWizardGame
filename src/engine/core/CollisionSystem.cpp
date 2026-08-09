#include "engine/core/CollisionSystem.hpp"
#include "engine/ecs/components/PhysicsComponents.hpp"
#include "engine/ecs/components/Transform.hpp"
void CollisionSystem::Init(entt::registry& registry) {
    worldDef = b3DefaultWorldDef();
    worldDef.gravity = b3Vec3{0.0f, -10.0f, 0.0f};

    worldId = b3CreateWorld(&worldDef);
    
    // Automatically collect entities when CollisionBoxComponent is added
        registry.on_construct<CollisionBoxComponent>().connect<&CollisionSystem::RegisterObjectCollisionBox>(this);
        registry.on_construct<CollisionSphereComponent>().connect<&CollisionSystem::RegisterObjectCollisionSphere>(this);
        registry.on_construct<CollisionCapsuleComponent>().connect<&CollisionSystem::RegisterObjectCollisionCapsule>(this);
        registry.on_construct<CollisionMeshComponent>().connect<&CollisionSystem::RegisterObjectCollisionMesh>(this);
        
        registry.on_destroy<CollisionMeshComponent>().connect<&CollisionSystem::OnDestroyCollisionMesh>(this);
        registry.on_destroy<RigidBodyComponent>().connect<&CollisionSystem::OnDestroyRigidBody>(this);
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
b3BodyId CollisionSystem::GetOrCreateBody(entt::registry& registry, entt::entity entity) {
    auto& rb = registry.get_or_emplace<RigidBodyComponent>(entity);
    if (b3Body_IsValid(rb.id)) {
        return rb.id;
    }

    Transform* transform = registry.try_get<Transform>(entity);
    b3BodyDef bodyDef = b3DefaultBodyDef();

    if (transform) {
        glm::vec3 pos = transform->position;
        glm::quat rot = glm::normalize(transform->rotation);
        bodyDef.position = b3Pos{pos.x, pos.y, pos.z};
        bodyDef.rotation = b3Quat{b3Vec3{rot.x, rot.y, rot.z}, rot.w};
    }

    bodyDef.type = (rb.type == BodyType::Dynamic) ? b3_dynamicBody : b3_staticBody;
    rb.id = b3CreateBody(worldId, &bodyDef);
    return rb.id;
}
void CollisionSystem::RegisterObjectCollisionBox(entt::registry& registry, entt::entity entity) {
    b3BodyId bodyId = GetOrCreateBody(registry, entity);
    auto& box = registry.get<CollisionBoxComponent>(entity);
    auto& rb = registry.get<RigidBodyComponent>(entity);
    Transform* transform = registry.try_get<Transform>(entity);

    glm::vec3 scale = transform ? transform->scale : glm::vec3{1.0f};
    glm::vec3 extents = box.halfExtents * scale;

    b3BoxHull boxHull = b3MakeBoxHull(extents.x, extents.y, extents.z);
    b3ShapeDef shapeDef = b3DefaultShapeDef();
    if (rb.type == BodyType::Dynamic) {
        shapeDef.density = 1.0f;
        shapeDef.baseMaterial.friction = 0.3f;
    }

    b3CreateHullShape(bodyId, &shapeDef, &boxHull.base);
}
void CollisionSystem::RegisterObjectCollisionSphere(entt::registry& registry, entt::entity entity) {
    b3BodyId bodyId = GetOrCreateBody(registry, entity);
    auto& sphereComp = registry.get<CollisionSphereComponent>(entity);
    auto& rb = registry.get<RigidBodyComponent>(entity);
    Transform* transform = registry.try_get<Transform>(entity);

    float scale = transform ? std::max({transform->scale.x, transform->scale.y, transform->scale.z}) : 1.0f;
    b3Sphere sphere{ b3Vec3{0.0f, 0.0f, 0.0f}, sphereComp.radius * scale };

    b3ShapeDef shapeDef = b3DefaultShapeDef();
    if (rb.type == BodyType::Dynamic) {
        shapeDef.density = 1.0f;
        shapeDef.baseMaterial.friction = 0.3f;
    }

    b3CreateSphereShape(bodyId, &shapeDef, &sphere);
}
void CollisionSystem::RegisterObjectCollisionCapsule(entt::registry& registry, entt::entity entity) {
    b3BodyId bodyId = GetOrCreateBody(registry, entity);
    auto& capsuleComp = registry.get<CollisionCapsuleComponent>(entity);
    auto& rb = registry.get<RigidBodyComponent>(entity);
    Transform* transform = registry.try_get<Transform>(entity);

    float scaleY = transform ? transform->scale.y : 1.0f;
    float scaleX = transform ? transform->scale.x : 1.0f;

    float halfHeight = (capsuleComp.length * scaleY) * 0.5f;
    float radius = capsuleComp.radius * scaleX;

    b3Capsule capsule{
        b3Vec3{0.0f, -halfHeight, 0.0f},
        b3Vec3{0.0f,  halfHeight, 0.0f},
        radius
    };

    b3ShapeDef shapeDef = b3DefaultShapeDef();
    if (rb.type == BodyType::Dynamic) {
        shapeDef.density = 1.0f;
        shapeDef.baseMaterial.friction = 0.3f;
    }

    b3CreateCapsuleShape(bodyId, &shapeDef, &capsule);
}
void CollisionSystem::RegisterObjectCollisionMesh(entt::registry& registry, entt::entity entity) {
    auto& meshComp = registry.get<CollisionMeshComponent>(entity);
    if (!meshComp.mesh || meshComp.mesh->vertices.empty()) return;

    b3BodyId bodyId = GetOrCreateBody(registry, entity);
    auto& rb = registry.get<RigidBodyComponent>(entity);
    Transform* transform = registry.try_get<Transform>(entity);

    glm::vec3 scale = transform ? transform->scale : glm::vec3{1.0f};

    // Bake transform scale into vertex positions
    std::vector<b3Vec3> b3Points;
    b3Points.reserve(meshComp.mesh->vertices.size());
    for (const auto& v : meshComp.mesh->vertices) {
        b3Points.push_back(b3Vec3{ v.position.x * scale.x, v.position.y * scale.y, v.position.z * scale.z });
    }

    b3ShapeDef shapeDef = b3DefaultShapeDef();
    if (rb.type == BodyType::Dynamic) {
        shapeDef.density = 1.0f;
        shapeDef.baseMaterial.friction = 0.4f;
    }

    if (meshComp.isConvex) {
        // 2 & 3 & 4. Convex Hull (Pass 3 args, pass 'hull' pointer directly, destroy afterward)
        int pointCount = static_cast<int>(b3Points.size());
        b3HullData* hull = b3CreateHull(b3Points.data(), pointCount, pointCount);

        if (hull != nullptr) {
            b3CreateHullShape(bodyId, &shapeDef, hull);
            b3DestroyHull(hull); // Free local allocation
        }
    } else {
        // 5. Static Triangle Mesh (Box3D 0.1.0 b3MeshDef + b3CreateMesh API)
        if (meshComp.mesh->indices.empty()) return;

        b3MeshDef def = {};
        def.vertices      = b3Points.data();
        def.vertexCount   = static_cast<int>(b3Points.size());
        def.indices       = (int32_t*)meshComp.mesh->indices.data();
        def.triangleCount = static_cast<int>(meshComp.mesh->indices.size() / 3);
        def.useMedianSplit = true;
        def.identifyEdges = true;
        def.weldVertices  = true;

        meshComp.meshData = b3CreateMesh(&def, nullptr, 0);
        if (meshComp.meshData != nullptr) {
            b3Vec3 meshScale{1.0f, 1.0f, 1.0f};
            b3CreateMeshShape(bodyId, &shapeDef, meshComp.meshData, meshScale);
        }
    }
}
void CollisionSystem::OnDestroyCollisionMesh(entt::registry& registry, entt::entity entity) {
    auto& meshComp = registry.get<CollisionMeshComponent>(entity);
    if (meshComp.meshData) {
        b3DestroyMesh(meshComp.meshData);
        meshComp.meshData = nullptr;
    }
}

void CollisionSystem::OnDestroyRigidBody(entt::registry& registry, entt::entity entity) {
    auto& rb = registry.get<RigidBodyComponent>(entity);
    if (b3Body_IsValid(rb.id)) {
        b3DestroyBody(rb.id);
        rb.id = b3_nullBodyId;
    }
}
void CollisionSystem::ApplyJump(entt::registry& registry, entt::entity entity, float jumpStrength) {
    auto* rb = registry.try_get<RigidBodyComponent>(entity);

    if (rb && b3Body_IsValid(rb->id) && rb->type == BodyType::Dynamic) {
        b3Vec3 impulse{0.0f, 0.0f, jumpStrength};
        
        // true = wake up body if it is sleeping
        b3Body_ApplyLinearImpulseToCenter(rb->id, impulse, true); 
    }
}
// Any object with a CollisionBoxComponent should add a corresponding b3BodyId + b3BodyDef

// Any object with a CollisionBoxComponent AND a RigidBodyComponent should add a corresponding:
// - b3BodyDef
// - b3BodyId
// - b3BoxHull
// - b3ShapeDef
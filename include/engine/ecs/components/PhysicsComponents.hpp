#ifndef COLLISIONBOXCOMPONENT_H
#define COLLISIONBOXCOMPONENT_H
#include <glm/glm.hpp>
#include <box3d/box3d.h>
#include "engine/render/Mesh.hpp"
enum class BodyType {
    Static,
    Dynamic,
};
struct CollisionBoxComponent {
    glm::vec3 halfExtents;
};
struct RigidBodyComponent {
    BodyType type = BodyType::Static;
    b3BodyId id = b3_nullBodyId;
};
struct CollisionSphereComponent {
    float radius = 0.5f;
};

struct CollisionCapsuleComponent {
    float radius = 0.5f;
    float length = 1.0f; // Height along Y axis
};

struct CollisionMeshComponent {
    const Mesh* mesh = nullptr;
    bool isConvex = true; // true = b3ComputeHull (dynamic/static), false = b3CreateMesh (static only)
    b3MeshData* meshData = nullptr;
};
#endif
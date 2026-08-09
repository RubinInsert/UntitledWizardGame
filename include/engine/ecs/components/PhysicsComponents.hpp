#ifndef COLLISIONBOXCOMPONENT_H
#define COLLISIONBOXCOMPONENT_H
#include <glm/glm.hpp>
#include <box3d/box3d.h>
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
#endif
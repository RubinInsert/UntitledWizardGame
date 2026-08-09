#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include <box3d/box3d.h>
#include <entt/entt.hpp>
class CollisionSystem {
public:
    void Init(entt::registry& registry);
    void Step(entt::registry& registry);
    void ApplyJump(entt::registry& registry, entt::entity entity, float jumpStrength);
private:
   b3WorldDef worldDef;
   b3WorldId worldId; 
   b3BodyId GetOrCreateBody(entt::registry& registry, entt::entity entity);
   /// @brief An event listening function that acts as a bridge between the ECS system and the Box3D Physics Library
   /// @param registry 
   /// @param ent
   void RegisterObjectCollisionBox(entt::registry& registry, entt::entity entity);
   void RegisterObjectCollisionSphere(entt::registry& registry, entt::entity entity);
   void RegisterObjectCollisionCapsule(entt::registry& registry, entt::entity entity);
   void RegisterObjectCollisionMesh(entt::registry& registry, entt::entity entity);
   void OnDestroyCollisionMesh(entt::registry& registry, entt::entity entity);
   void OnDestroyRigidBody(entt::registry& registry, entt::entity entity);
};


#endif
#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include <box3d/box3d.h>
#include <entt/entt.hpp>
class CollisionSystem {
public:
    void Init(entt::registry& registry);
    void Step(entt::registry& registry);
private:
   b3WorldDef worldDef;
   b3WorldId worldId; 

   /// @brief An event listening function that acts as a bridge between the ECS system and the Box3D Physics Library
   /// @param registry 
   /// @param ent
   void RegisterObjectCollisionBox(entt::registry& registry, entt::entity ent);
};


#endif
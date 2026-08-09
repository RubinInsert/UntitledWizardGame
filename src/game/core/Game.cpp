#include "game/core/Game.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/render/SpriteSheet.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
#include "game/world/TileMap.hpp"
#include "engine/core/InputManager.hpp"
#include "game/core/CameraSystem.hpp"
#include "engine/ecs/components/Animation.hpp"
#include "engine/ecs/components/AnimationSequence.hpp"
#include "game/player/PlayerAnims.hpp"
#include "engine/render/DebugDrawer.hpp"
#include "engine/render/Texture.hpp"
#include "engine/ecs/components/MeshComponent.hpp"
#include "engine/ecs/components/PhysicsComponents.hpp"
#include <vector>
Game::Game(Engine& engine)
    : engine(engine)
{
    createScene();
}
void Game::createScene() {
        // Quick test: load cached barrel and submit
    Mesh* barrelMesh = engine.getAssetManager().getModel("barrel");
    entt::entity barrel = engine.getRegistry().create();
    if (barrelMesh) {
        Transform t;
        t.position = glm::vec3(0.0f, 50.0f, 0.0f);
        t.scale    = glm::vec3(1.0f);
        glm::vec3 eulerDegrees{30.0f, 0.0f, 0.0f};
        t.rotation = glm::quat(glm::radians(eulerDegrees));
        engine.getRegistry().emplace<Transform>(barrel, t);
        engine.getRegistry().emplace<MeshComponent>(barrel, MeshComponent{barrelMesh});
        engine.getRegistry().emplace<RigidBodyComponent>(barrel, RigidBodyComponent{BodyType::Dynamic});
        engine.getRegistry().emplace<CollisionMeshComponent>(barrel, CollisionMeshComponent{barrelMesh});
        //renderSystem.SubmitMesh(barrel, t);
    }

    Mesh* groundMesh = engine.getAssetManager().getModel("sample_terrain");
    entt::entity ground = engine.getRegistry().create();
    if (groundMesh) {
        Transform t;
        t.position = glm::vec3(10.0f, 0.0f, 0.0f);
        t.scale    = glm::vec3(1.0f);
        engine.getRegistry().emplace<Transform>(ground, t);
        engine.getRegistry().emplace<MeshComponent>(ground, MeshComponent{groundMesh});
        engine.getRegistry().emplace<CollisionMeshComponent>(ground, CollisionMeshComponent{groundMesh, false});
        
        //renderSystem.SubmitMesh(barrel, t);
    }

    engine.getCollisionSystem().ApplyJump(engine.getRegistry(), barrel, 1.0f);
            // std::string imagePath {"assets/snail.png"};
            // std::string playerAssetPath {"assets/magician.png"};
            // player = registry.create();
            // Mesh* testBarrel = engine->getAssetManager().getModel("barrel");
            // // Load texture from asset manager
            // Texture* texture = engine.getAssetManager().getTexture(playerAssetPath);
            // float width = texture->width, height = texture->height;
            
            // // Choose frame from sprite
            // float frameWidth = width / 8.f;
            // float frameHeight = height / 8.f;
            // // Create sprite component with loaded texture
            // SpriteSheet* spriteSheet = engine.getAssetManager().getSpriteSheet("assets/male_unarmored.png", 128, 128, 8, 8, 64, 64, 128, 128);
            // registry.emplace<Sprite>(player, Sprite{spriteSheet, 16});  // frame 0
            
            // // Create transform component
            // registry.emplace<Transform>(player, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{0.75f, 0.75f}});
            // registry.emplace<Velocity>(player, Velocity{SDL_FPoint{0.f, 0.f}});
            // registry.emplace<MovementStats>(player, MovementStats{5.f});
            // registry.emplace<Animation>(player, 
            //     PlayerAnims::IDLE_NORTH, // startFrame, frameCount, duration, loop
            //     0.0f, // timer
            //     0     // currentFrameOffset
            // );
            // auto entity2 = registry.create();
            
            // // Load texture from asset manager
            // SDL_Texture* texture2 = assetManager.getTexture(imagePath);
            // float width2 = 0.f, height2 = 0.f;
            // if (texture2) SDL_GetTextureSize(texture2, &width2, &height2);
            
            // SpriteSheet* spriteSheet2 = assetManager.getSpriteSheet("assets/snail.png", width2, height2, 1, 1, 0.f, 0.f, 0.f, 0.f);
            // // Create sprite component with loaded texture
            // registry.emplace<Sprite>(entity2, Sprite{spriteSheet2, 0});

            // Create transform component
            //registry.emplace<Transform>(entity2, Transform{SDL_FPoint{1.f, 2.f}, SDL_FPoint{width2, height2}});



            // TILE MAP LOADING TESTING
            // Load the map from Tiled JSO
            //Game::collisionLayer = loadedCollisionLayer;
}

void Game::update () {
    animatorSystem.update(engine.getRegistry(), engine.getTimeManager().getDeltaTime());
    playerController.update(engine.getRegistry(), player, engine.getInputManager(), engine.getTimeManager().getDeltaTime());
    // If left mouse button is down, allow CameraSystem to handle dragging
    RenderSystem& renderSys = engine.getRenderSystem();
    cameraSystem.update(renderSys.getCamera(), engine.getInputManager());  // 3D camera
}

void Game::shutdown() {

}
void Game::render(RenderSystem& renderSystem) {
    worldMap.tileMap.render(renderSystem, camera, engine.getWorldSettings());

   
}
const Camera& Game::getCamera() const {
    return camera;
}
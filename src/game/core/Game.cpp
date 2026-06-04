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
#include "game/world/MapLoader.hpp"
#include "game/player/PlayerAnims.hpp"
#include "engine/render/DebugDrawer.hpp"
#include "engine/render/Texture.hpp"
#include <vector>
Game::Game(Engine& engine)
    : engine(engine)
{
    camera.position = {0.f, 0.f};
    camera.zoom = 1.f;
    camera.viewportWidth = engine.getScreenWidth();
    camera.viewportHeight = engine.getScreenHeight();
    createScene();
}
void Game::createScene() {
            camera.zoom = 2.5f;
            std::string imagePath {"assets/snail.png"};
            std::string playerAssetPath {"assets/magician.png"};
            player = registry.create();
            // Load texture from asset manager
            Texture* texture = engine.getAssetManager().getTexture(playerAssetPath);
            float width = texture->width, height = texture->height;
            
            // Choose frame from sprite
            float frameWidth = width / 8.f;
            float frameHeight = height / 8.f;
            // Create sprite component with loaded texture
            SpriteSheet* spriteSheet = engine.getAssetManager().getSpriteSheet("assets/male_unarmored.png", 128, 128, 8, 8, 64, 64, 128, 128);
            registry.emplace<Sprite>(player, Sprite{spriteSheet, 16});  // frame 0
            
            // Create transform component
            registry.emplace<Transform>(player, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{0.75f, 0.75f}});
            registry.emplace<Velocity>(player, Velocity{SDL_FPoint{0.f, 0.f}});
            registry.emplace<MovementStats>(player, MovementStats{5.f});
            registry.emplace<Animation>(player, 
                PlayerAnims::IDLE_NORTH, // startFrame, frameCount, duration, loop
                0.0f, // timer
                0     // currentFrameOffset
            );
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
            // Load the map from Tiled JSON
            worldMap = MapLoader::Load("assets/worldData/maps/map01.json", engine.getAssetManager(), registry);
            //Game::collisionLayer = loadedCollisionLayer;
}

void Game::update () {
    animatorSystem.update(registry, engine.getTimeManager().getDeltaTime());
    playerController.update(registry, player, engine.getInputManager(), engine.getTimeManager().getDeltaTime());
    physics.update(registry, engine.getTimeManager().getDeltaTime());
    collisions.update(registry, worldMap.tileMap);
    // If left mouse button is down, allow CameraSystem to handle dragging
    if (engine.getInputManager().isMouseButtonDown(SDL_BUTTON_LEFT)) {
        cameraSystem.update(camera, engine.getInputManager());
        return;
    }
    // Otherwise, follow the player transform if available
    if (player != entt::null && registry.valid(player) && registry.all_of<Transform>(player)) {
        const Transform& playerTransform = registry.get<Transform>(player);
        cameraSystem.followPlayer(camera, playerTransform, engine.getTimeManager(), engine.getWorldSettings());
    } else {
        // fallback test target
        cameraSystem.followPlayer(camera, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{5.f, 5.f}}, engine.getTimeManager(), engine.getWorldSettings());
    }
}

void Game::shutdown() {

}
void Game::render(SDL_Renderer& renderer) {
    worldMap.tileMap.render(renderer, camera, engine.getWorldSettings());
}
entt::registry& Game::getRegistry() {
    return registry;
}
const Camera& Game::getCamera() const {
    return camera;
}
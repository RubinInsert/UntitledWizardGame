#include "game/core/Game.hpp"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/ecs/components/Velocity.hpp"
#include "game/components/MovementStats.hpp"
#include "game/world/TileMap.hpp"
#include "engine/core/InputManager.hpp"
#include "game/core/CameraSystem.hpp"
Game::Game(AssetManager& assetManager, InputManager& inputManager, TimeManager& time, WorldSettings& worldSettings, float viewportWidth, float viewportHeight)
    : assetManager(assetManager)
    , inputManager(inputManager)
    , time(time)
    , worldSettings(worldSettings)
    , tileMap(0, 0, nullptr)
{
    createScene();
    camera.position = {0.f, 0.f};
    camera.zoom = 1.f;
    camera.viewportWidth = viewportWidth;
    camera.viewportHeight = viewportHeight;
}
void Game::createScene() {
            std::string imagePath {"assets/snail.png"};
            std::string playerAssetPath {"assets/one_magician.png"};
            player = registry.create();
            // Load texture from asset manager
            SDL_Texture* texture = assetManager.getTexture(playerAssetPath);
            float width = 0.f, height = 0.f;
            if (texture) SDL_GetTextureSize(texture, &width, &height);
            
            // Create sprite component with loaded texture
            registry.emplace<Sprite>(player, Sprite{texture, SDL_FRect{0.f, 0.f, width, height}});
            
            // Create transform component
            registry.emplace<Transform>(player, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{width, height}});
            registry.emplace<Velocity>(player, Velocity{SDL_FPoint{0.f, 0.f}});
            registry.emplace<MovementStats>(player, MovementStats{5.f});

            auto entity2 = registry.create();
            
            // Load texture from asset manager
            SDL_Texture* texture2 = assetManager.getTexture(imagePath);
            float width2 = 0.f, height2 = 0.f;
            if (texture2) SDL_GetTextureSize(texture2, &width2, &height2);

            // Create sprite component with loaded texture
            registry.emplace<Sprite>(entity2, Sprite{texture2, SDL_FRect{0.f, 0.f, width2, height2}});

            // Create transform component
            registry.emplace<Transform>(entity2, Transform{SDL_FPoint{1.f, 2.f}, SDL_FPoint{width2, height2}});

            SDL_Texture* tex = assetManager.getTexture("assets/one_tile.png");
            tileMap = TileMap(32, 32, tex);
}

void Game::update () {

    playerController.update(registry, player, inputManager, time.getDeltaTime());
    physics.update(registry, time.getDeltaTime());


    // If left mouse button is down, allow CameraSystem to handle dragging
    if (inputManager.isMouseButtonDown(SDL_BUTTON_LEFT)) {
        cameraSystem.update(camera, inputManager);
        return;
    }
    // Otherwise, follow the player transform if available
    if (player != entt::null && registry.valid(player) && registry.all_of<Transform>(player)) {
        const Transform& playerTransform = registry.get<Transform>(player);
        cameraSystem.followPlayer(camera, playerTransform, time, worldSettings);
    } else {
        // fallback test target
        cameraSystem.followPlayer(camera, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{5.f, 5.f}}, time, worldSettings);
    }
}

void Game::shutdown() {

}
void Game::render(SDL_Renderer& renderer) {
    tileMap.render(renderer, camera, worldSettings);
}
entt::registry& Game::getRegistry() {
    return registry;
}
const Camera& Game::getCamera() const {
    return camera;
}
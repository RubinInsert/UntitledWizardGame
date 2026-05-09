#ifndef GAME_H
#define GAME_H
#include <entt/entt.hpp>
#include <SDL3/SDL.h>
#include "engine/core/InputManager.hpp"
#include "engine/render/Camera.hpp"
#include "engine/core/AssetManager.hpp"
#include "engine/core/TimeManager.hpp"
#include "engine/core/WorldSettings.hpp"
#include "game/core/CameraSystem.hpp"
#include "game/player/PlayerController.hpp"
#include "game/movement/PhysicsSystem.hpp"
#include "game/movement/CollisionSystem.hpp"
#include "game/world/TileMap.hpp"
class Game {
    public:
        Game(AssetManager& assetManager, InputManager& inputManager, TimeManager& time, WorldSettings& worldSettings, float viewportWidth, float viewportHeight);
        bool init (AssetManager& assetManager, InputManager& inputManager, TimeManager& time);
        void update();
        void shutdown();
        void render(SDL_Renderer& renderer);
        entt::registry& getRegistry();
        const Camera& getCamera() const;

    private:
        void createScene();
        void updatePlayer(float deltaTime, const InputManager& input);
        void updateCamera(float deltaTime, const InputManager& input);

    entt::registry registry;
    CameraSystem cameraSystem{};
    Camera camera{};
    PlayerController playerController;
    PhysicsSystem physics;
    CollisionSystem collisions;
    entt::entity player{entt::null};
    AssetManager& assetManager;
    InputManager& inputManager;
    TimeManager& time;
    WorldSettings& worldSettings;
    TileMap tileMap;

};

#endif
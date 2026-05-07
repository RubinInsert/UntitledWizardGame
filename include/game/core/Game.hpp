#ifndef GAME_H
#define GAME_H
#include <entt/entt.hpp>
#include "engine/core/InputManager.hpp"
#include "engine/render/Camera.hpp"
#include "engine/core/AssetManager.hpp"
#include "game/core/CameraSystem.hpp"
class Game {
    public:
        bool init (AssetManager& assetManager, InputManager& inputManager);
        void update();
        void shutdown();
        
        entt::registry& getRegistry();
        const Camera& getCamera() const;

    private:
        void createScene();
        void updatePlayer(float deltaTime, const InputManager& input);
        void updateCamera(float deltaTime, const InputManager& input);

    entt::registry registry;
    CameraSystem cameraSystem{};
    Camera camera{};
    entt::entity player{entt::null};
    AssetManager* assetManager{nullptr};
    InputManager* inputManager{nullptr};

};

#endif
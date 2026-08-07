#ifndef GAME_H
#define GAME_H
#include <entt/entt.hpp>
#include <SDL3/SDL.h>
#include "engine/core/Engine.h"
#include "engine/render/Camera.hpp"
#include "engine/core/AnimatorSystem.hpp"
#include "game/core/CameraSystem.hpp"
#include "game/player/PlayerController.hpp"
#include "game/world/Map.hpp"
class Game {
    public:
        Game(Engine& engine);
        bool init ();
        void update();
        void shutdown();
        void render(RenderSystem& renderSystem);
        const Camera& getCamera() const;

    private:
        void createScene();
        void updatePlayer(float deltaTime, const InputManager& input);
        void updateCamera(float deltaTime, const InputManager& input);


    CameraSystem cameraSystem{};
    Camera camera{};
    PlayerController playerController;
    AnimatorSystem animatorSystem;
    entt::entity player{entt::null};
    Engine& engine;
    Map worldMap;

};

#endif
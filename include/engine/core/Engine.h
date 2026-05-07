#ifndef ENGINE_H
#define ENGINE_H
#include "engine/core/WindowManager.hpp"
#include "engine/core/AssetManager.hpp"
#include "engine/render/SpriteRenderer.hpp"
#include "engine/core/InputManager.hpp"
#include "engine/core/TimeManager.hpp"
#include <entt/entt.hpp>
class Game;
class Engine {
    public:
        Engine();
        bool Init();
        int Run(Game& game);

        AssetManager& getAssetManager();
        InputManager& getInputManager();
        TimeManager& getTimeManager();
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        InputManager inputManager;
        TimeManager timeManager;
        SpriteRenderer spriteRenderer;
};

#endif
#ifndef ENGINE_H
#define ENGINE_H
#include "engine/core/WindowManager.hpp"
#include "engine/core/AssetManager.hpp"
#include "engine/render/SpriteRenderer.hpp"
#include "engine/core/InputManager.hpp"
#include <entt/entt.hpp>
class Game;
class Engine {
    public:
        Engine();
        bool Init();
        int Run(Game& game);

        AssetManager& getAssetManager();
        InputManager& getInputManager();
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        InputManager inputManager;
        SpriteRenderer spriteRenderer;
};

#endif
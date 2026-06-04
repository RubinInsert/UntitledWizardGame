#ifndef ENGINE_H
#define ENGINE_H
#include "engine/core/WindowManager.hpp"
#include "engine/core/AssetManager.hpp"
#include "engine/render/SpriteRenderer.hpp"
#include "engine/core/InputManager.hpp"
#include "engine/core/TimeManager.hpp"
#include "engine/core/WorldSettings.hpp"
#include <entt/entt.hpp>
#include "engine/render/RenderSystem.hpp"
class Game;
class Engine {
    public:
        Engine(int screenWidth, int screenHeight);
        bool Init();
        int Run(Game& game);

        AssetManager& getAssetManager();
        InputManager& getInputManager();
        TimeManager& getTimeManager();
        RenderSystem& getRenderSystem();
        WorldSettings& getWorldSettings();
        int getScreenWidth();
        int getScreenHeight();
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        InputManager inputManager;
        TimeManager timeManager;
        RenderSystem renderSystem;
        SpriteRenderer spriteRenderer;
        WorldSettings worldSettings;
        int screenWidth;
        int screenHeight;
};

#endif
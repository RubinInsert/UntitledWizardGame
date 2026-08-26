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
#include "engine/core/CollisionSystem.hpp"
#include "engine/core/IApplication.hpp"
class Engine {
    public:
        Engine(int screenWidth, int screenHeight);
        bool Init();
        int Run(IApplication& game);

        AssetManager& getAssetManager();
        InputManager& getInputManager();
        TimeManager& getTimeManager();
        RenderSystem& getRenderSystem();
        SDL_GPUDevice* getGPUDevice();
        WindowManager& getWindowManager();
        CollisionSystem& getCollisionSystem();
        WorldSettings& getWorldSettings();
        int getScreenWidth();
        int getScreenHeight();
        entt::registry& getRegistry();
    private:
        WindowManager windowManager;
        AssetManager assetManager;
        InputManager inputManager;
        TimeManager timeManager;
        RenderSystem renderSystem;
        SDL_GPUDevice* gpuDevice;
        CollisionSystem collisionSystem;
        SpriteRenderer spriteRenderer;
        WorldSettings worldSettings;
        entt::registry registry;
        int screenWidth;
        int screenHeight;
};

#endif
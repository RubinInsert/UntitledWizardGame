#include "engine/core/Engine.h"
#include "engine/ecs/components/Sprite.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/render/SpriteRenderer.hpp"
#include "engine/render/DebugDrawer.hpp"
#include "game/core/Game.hpp"
#include <SDL3/SDL.h>
constexpr int kScreenWidth{ 1280 };
constexpr int kScreenHeight{ 960 };
Engine::Engine(int screenWidth, int screenHeight): windowManager{}, assetManager{nullptr}, inputManager{}, spriteRenderer{}, screenWidth(screenWidth), screenHeight(screenHeight) {}
bool Engine::Init() {
    bool success {true};
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        success = false;
    } else {
        // Initialization Successful
        // Create window
        if( windowManager.createWindow("Untitled Wizard Game", kScreenWidth, kScreenHeight) == false ) {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
        SDL_SetHint(SDL_HINT_RENDER_GPU_DEBUG, "1");
        SDL_GPUDevice* gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, true, nullptr);
        if (!gpuDevice) {
            SDL_Log("Failed to create GPU device: %s", SDL_GetError());
            return false;
        }
        if (!SDL_ClaimWindowForGPUDevice(gpuDevice, windowManager.getWindow())) {
            SDL_Log("Failed to claim window: %s", SDL_GetError());
            return false;
        }
        // Link Newly created GPU device to Systems
        assetManager.setGPUDevice(gpuDevice);
        assetManager.Init();
        assetManager.resolveRegistry();
        renderSystem.setGPUDevice(gpuDevice);
        renderSystem.setTargetWindow(windowManager.getWindow());
        renderSystem.initResources(screenWidth, screenHeight, *this); 
    }
    return success;
}
int Engine::Run(Game& game) {
            Uint64 lastTime = SDL_GetPerformanceCounter();
            int frameCount = 0;
            double fpsTimer = 0.0;
            while (inputManager.shouldQuit() == false) {
                Uint64 now = SDL_GetPerformanceCounter();
                double delta = double(now - lastTime) / SDL_GetPerformanceFrequency();
                lastTime = now;
                fpsTimer += delta;
                frameCount++;

                if (fpsTimer >= 1.0) {
                    SDL_Log("FPS: %d", frameCount);
                    frameCount = 0;
                    fpsTimer = 0.0;
                }
                // Clear any previously drawn debug boxes
               // DebugDrawer::Clear();
                // Update elapsed and delta times.
                timeManager.update();

                // Register inputs
                inputManager.update();

                game.update();
                // Fill the surface white
                // SDL_SetRenderDrawColor(windowManager.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
                // SDL_RenderClear(windowManager.getRenderer());
                
                game.render(renderSystem);
                spriteRenderer.render(registry, renderSystem, game.getCamera(), worldSettings);
                renderSystem.render();
                // Render additional game renders
                // Render sprites through the sprite renderer

                // Debug draws
                //DebugDrawer::DrawIsometric(windowManager.getRenderer(), game.getCamera(), worldSettings);
                // Update the screen
                //SDL_RenderPresent(windowManager.getRenderer());
            }
    return 0;
}

AssetManager& Engine::getAssetManager() {
    return assetManager;
}
InputManager& Engine::getInputManager() {
    return inputManager;
}
TimeManager& Engine::getTimeManager() {
    return timeManager;
}
RenderSystem& Engine::getRenderSystem() {
    return renderSystem;
}
WorldSettings& Engine::getWorldSettings() {
    return worldSettings;
}
int Engine::getScreenWidth() {
    return screenWidth;
}
int Engine::getScreenHeight() {
    return screenHeight;
}
entt::registry& Engine::getRegistry() {
    return registry;
}
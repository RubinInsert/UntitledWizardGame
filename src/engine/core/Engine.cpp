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
        // Link Newly created renderer to Asset manager
        assetManager.setRenderer(windowManager.getRenderer());
    }
    return success;
}
int Engine::Run(Game& game) {
            while (inputManager.shouldQuit() == false) {
                // Clear any previously drawn debug boxes
                DebugDrawer::Clear();
                // Update elapsed and delta times.
                timeManager.update();

                // Register inputs
                inputManager.update();

                game.update();
                // Fill the surface white
                SDL_SetRenderDrawColor(windowManager.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(windowManager.getRenderer());
                
                // Render additional game renders
                game.render(*windowManager.getRenderer());
                // Render sprites through the sprite renderer
                spriteRenderer.render(game.getRegistry(), windowManager.getRenderer(), game.getCamera(), worldSettings);

                // Debug draws
                DebugDrawer::DrawIsometric(windowManager.getRenderer(), game.getCamera(), worldSettings);
                // Update the screen
                SDL_RenderPresent(windowManager.getRenderer());
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
WorldSettings& Engine::getWorldSettings() {
    return worldSettings;
}
int Engine::getScreenWidth() {
    return screenWidth;
}
int Engine::getScreenHeight() {
    return screenHeight;
}
#include "engine/core/WindowManager.hpp"
#include <string>
WindowManager::WindowManager(): gameWindow {nullptr}, gameRenderer {nullptr} {}
WindowManager::~WindowManager() {
    destroyAllWindows();
}
bool WindowManager::createWindow(std::string windowTitle, int windowWidth, int windowHeight) {
        bool success {true};
        gameWindow = SDL_CreateWindow(
        windowTitle.c_str(), 
        windowWidth, 
        windowHeight, 
        SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
        if( gameWindow == nullptr) {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
        return success;
}

SDL_Window* WindowManager::getWindow() {
    return gameWindow;
}

SDL_Renderer* WindowManager::getRenderer() {
    return gameRenderer;
}
void WindowManager::destroyAllWindows() {
        // Destroy Renderer
    SDL_DestroyRenderer( gameRenderer );
    gameRenderer = nullptr;

    // Destroy window
    SDL_DestroyWindow( gameWindow ); // Auto cleans up surface associated with window
    gameWindow = nullptr;
}
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "engine/core/Engine.h"
#include "editor/core/Editor.hpp"
#include <string>
// The window to be rendered to
SDL_Window* gWindow{ nullptr };

// The surface contained by the window
SDL_Renderer* gRenderer{ nullptr };

//Screen dimension constants
constexpr int kScreenWidth{ 1280 };
constexpr int kScreenHeight{ 940 };
void close () {

    // Destroy Renderer
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;

    // Destroy window
    SDL_DestroyWindow(gWindow); // Auto cleans up surface associated with window
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();

}
int main (int argc, char* argv[]) {
    Engine engine(kScreenWidth, kScreenHeight);
    // Initialize
    if ( engine.Init() == false)  {
        SDL_Log("Unable to initalize program!");
        return 1;
    }
    Editor editor(engine);
    // Load Media
    engine.Run(editor);
    close();

    return 0;
}
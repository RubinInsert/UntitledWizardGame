#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Engine.h"
#include <string>
// The window to be rendered to
SDL_Window* gWindow{ nullptr };

// The surface contained by the window
SDL_Renderer* gRenderer{ nullptr };

//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };
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
    // Final exit code
    int exitCode {0};
    Engine game;
    // Initialize
    if ( game.Init() == false)  {
        SDL_Log("Unable to initalize program!");
        exitCode = 1;
    } else {
        // Load Media
        game.Run();
        close();
        // if(game.Run() == false) {
        //     SDL_Log("Unable to load media!\n");
        //     exitCode = 2;
        // } else {
            
        //     close();
        // }
    }
    return 0;
}
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "LTexture.h"
#include <string>
// The window to be rendered to
SDL_Window* gWindow{ nullptr };

// The surface contained by the window
SDL_Renderer* gRenderer{ nullptr };

// Image to load on the screen
LTexture gPngTexture;
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };
bool init() {
bool success {true};

// Initialize SDL
if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
    success = false;
} else {
    // Initialization Successful
    // Create window
    if( SDL_CreateWindowAndRenderer("Untitled Wizard Game", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false ) {
        SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
        success = false;
    }
}
return success;
}
bool loadMedia() {
    bool success {true};

    // Load image
    std::string imagePath {"assets/snail.bmp"};
    if( gPngTexture.loadFromFile("assets/snail.png") == false ) {
        SDL_Log( "Unable to load image %s! SDL error: %s\n", imagePath.c_str(), SDL_GetError() );
        success = false;
    }
    return success;
}
void close () {
    // Cleanup texture 
    gPngTexture.destroy();

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

    // Initialize
    if ( init() == false)  {
        SDL_Log("Unable to initalize program!");
        exitCode = 1;
    } else {
        // Load Media
        if(loadMedia() == false) {
            SDL_Log("Unable to load media!\n");
            exitCode = 2;
        } else {
            bool quit {false};
            // The event data
            SDL_Event e;
            SDL_zero(e);
            while (quit == false) {
                while (SDL_PollEvent(&e) == true) {
                    // If event is quit type
                    if(e.type == SDL_EVENT_QUIT) {
                        quit = true;
                    }

                    // Fill the surface white
                    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(gRenderer);

                    // Render image on screen
                    gPngTexture.render(0.f, 0.f);

                    // Update the screen
                    SDL_RenderPresent(gRenderer);
                }
            }
            close();
        }
    }
    return 0;
}
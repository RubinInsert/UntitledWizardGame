#include "Engine.h"
#include "Sprite.hpp"
#include "SpriteRenderer.hpp"
#include <SDL3/SDL.h>
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };
Engine::Engine(): windowManager{}, assetManager{nullptr}, spriteRenderer{} {}
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
int Engine::Run() {
    bool quit {false};
            // The event data
            SDL_Event e;
            SDL_zero(e);
            std::string imagePath {"assets/snail.png"};
            Sprite testSprite(imagePath, assetManager, 0.f, 0.f);
            spriteRenderer.addSprite(&testSprite);
            while (quit == false) {
                while (SDL_PollEvent(&e) == true) {
                    // If event is quit type
                    if(e.type == SDL_EVENT_QUIT) {
                        quit = true;
                    }

                }
                // Fill the surface white
                SDL_SetRenderDrawColor(windowManager.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(windowManager.getRenderer());

                // Render image on screen
                spriteRenderer.render(windowManager.getRenderer());

                // Update the screen
                SDL_RenderPresent(windowManager.getRenderer());
            }
    return 0;
}
#include "Engine.h"
#include "Sprite.hpp"
#include "Transform.hpp"
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
            auto entity = registry.create();
            
            // Load texture from asset manager
            SDL_Texture* texture = assetManager.getTexture(imagePath);
            float width = 0.f, height = 0.f;
            if (texture) SDL_GetTextureSize(texture, &width, &height);
            
            // Create sprite component with loaded texture
            registry.emplace<Sprite>(entity, Sprite{texture, SDL_FRect{0.f, 0.f, width, height}});
            
            // Create transform component
            registry.emplace<Transform>(entity, Transform{SDL_FPoint{0.f, 0.f}, SDL_FPoint{width, height}});

            auto entity2 = registry.create();
            
            // Load texture from asset manager
            SDL_Texture* texture2 = assetManager.getTexture(imagePath);
            float width2 = 0.f, height2 = 0.f;
            if (texture) SDL_GetTextureSize(texture, &width2, &height2);
            
            // Create sprite component with loaded texture
            registry.emplace<Sprite>(entity2, Sprite{texture, SDL_FRect{0.f, 0.f, width2, height2}});
            
            // Create transform component
            registry.emplace<Transform>(entity2, Transform{SDL_FPoint{5.f, 128.f}, SDL_FPoint{width2, height2}});
            while (quit == false) {
                while (SDL_PollEvent(&e) == true) {
                    // If event is quit type
                    if(e.type == SDL_EVENT_QUIT) {
                        quit = true;
                    }
                    if(e.type == SDL_EVENT_KEY_DOWN) {
                        if(e.key.key == SDLK_UP) {
                            camera.zoom += 0.01;  // Zoom in
                        }
                        if(e.key.key == SDLK_DOWN) {
                            camera.zoom -= 0.01;  // Zoom out
                        }
                        if(e.key.key == SDLK_LEFT) {
                            camera.position.x += 1.f;
                        }
                        if(e.key.key == SDLK_RIGHT) {
                            camera.position.x -= 1.f;
                        }
                    }
                }
                // Fill the surface white
                SDL_SetRenderDrawColor(windowManager.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(windowManager.getRenderer());

                // Render sprites through the sprite renderer
                spriteRenderer.render(registry, windowManager.getRenderer(), camera);

                // Update the screen
                SDL_RenderPresent(windowManager.getRenderer());
            }
    return 0;
}
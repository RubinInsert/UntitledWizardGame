#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <queue>
#include "engine/ecs/components/Sprite.hpp"
#include "engine/render/Texture.hpp"
enum RenderPass {
    HeightMap,
    Shadow,
    Albedo
};
struct RenderSprite {
    Texture* texture;
    SDL_FRect sourceRect;
    SDL_FRect destRect;
    int layer;
    SDL_Color colorMod;
};
class RenderSystem {
    public:
        RenderSystem(SDL_GPUDevice* device);
        RenderSystem();
        ~RenderSystem();
        // Initialization Functions
        void setGPUDevice(SDL_GPUDevice* device);
        void setTargetWindow(SDL_Window* window);
        void initResources(int width, int height);
        // Rendering functions
        void render();
        void draw(const RenderSprite& sprite);
    private:
        SDL_GPUDevice* device;
        SDL_Window* targetWindow;
        bool resourcesInitialized = false;

        // Represents a FIFO draw queue of Sprite objects 
        std::queue<RenderSprite> spriteQueue;

        // GPU Resources
        SDL_GPUTexture* heightBuffer; // Render target for the HeightMap Pass
        SDL_GPUTexture* shadowBuffer;

        // Internal Pass Methods
        void runHeightMapPass(SDL_GPUCommandBuffer* cmd);
        void runShadowPass(SDL_GPUCommandBuffer* cmd);
        void runAlbedoPass(SDL_GPUCommandBuffer* cmd);

        // Internal Sprite rendering
        void processSpriteQueue(SDL_GPURenderPass* renderPass);
};

#endif
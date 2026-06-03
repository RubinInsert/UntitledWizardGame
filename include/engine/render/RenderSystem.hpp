#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <vector>
#include "engine/ecs/components/Sprite.hpp"
enum RenderPass {
    HeightMap,
    Shadow,
    Albedo
};

class RenderSystem {
    public:
        RenderSystem(SDL_GPUDevice* device);
        RenderSystem();
        ~RenderSystem();

        void setGPUDevice(SDL_GPUDevice* device);
        void setTargetWindow(SDL_Window* window);
        void initResources(int width, int height);

        void render();
        void registerSprite();
    private:
        SDL_GPUDevice* device;
        SDL_Window* targetWindow;
        bool resourcesInitialized = false;

        std::vector<Sprite> sprites;

        // GPU Resources
        SDL_GPUTexture* heightBuffer; // Render target for the HeightMap Pass
        SDL_GPUTexture* shadowBuffer;

        // Internal Pass Methods
        void runHeightMapPass(SDL_GPUCommandBuffer* cmd);
        void runShadowPass(SDL_GPUCommandBuffer* cmd);
        void runAlbedoPass(SDL_GPUCommandBuffer* cmd);
};

#endif
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
struct PositionTextureVertex {
    float pos[2];     // position (2 floats)
    float uv[2];        // texture coordinates (2 floats)
    float color[4]; // colour (4 floats)
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
        SDL_GPUBuffer* vertexBuffer;  // For dynamic vertex data
        SDL_GPUBuffer* indexBuffer;  // For dynamic index data
        SDL_GPUBuffer* uniformBuffer; // For projection matrix
        SDL_GPUSampler* defaultSampler;
        SDL_GPUGraphicsPipeline* spritePipeline;

        // Shaders are to be moved and owned by the AssetManager
        SDL_GPUShader* vertexShader;
        SDL_GPUShader* fragmentShader;
        SDL_GPUShader* LoadShader(SDL_GPUDevice* device,
            const char* shaderFilename,
	        Uint32 samplerCount,
            Uint32 uniformBufferCount,
            Uint32 storageBufferCount,
            Uint32 storageTextureCount);
        // Internal Pass Methods
        void runHeightMapPass(SDL_GPUCommandBuffer* cmd);
        void runShadowPass(SDL_GPUCommandBuffer* cmd);
        void runAlbedoPass(SDL_GPUCommandBuffer* cmd);

        // Internal Sprite rendering
        void processSpriteQueue(SDL_GPURenderPass* renderPass);
        void paintSprite(SDL_GPURenderPass* renderPass, const RenderSprite& sprite);
        bool createShaders();
        bool createGraphicsPipeline();        
};
#endif
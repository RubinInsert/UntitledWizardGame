#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "engine/render/Mesh.hpp"
#include "Camera.hpp"
#include "engine/ecs/components/Transform.hpp"
class Engine;

struct Matrix4x4 {
    float m[16];
};

struct MeshRenderCommand {
    Mesh* mesh;
    Transform transform;
};

class RenderSystem {
    public:
        RenderSystem(SDL_GPUDevice* device);
        RenderSystem();
        ~RenderSystem();
        // Initialization Functions
        void setGPUDevice(SDL_GPUDevice* device);
        void setTargetWindow(SDL_Window* window);
        void initResources(int width, int height, Engine& eng);
        // Rendering functions
        void render();

        Camera& getCamera() { return camera; }
        bool SubmitMesh(Mesh* mesh, const Transform& transform);
    private:
        Engine* engine;
        SDL_GPUDevice* device;
        SDL_Window* targetWindow;
        bool resourcesInitialized = false;

        // 3D Resources
        SDL_GPUBuffer* cubeVertexBuffer = nullptr;
        SDL_GPUBuffer* cubeIndexBuffer = nullptr;
        SDL_GPUGraphicsPipeline* meshPipeline = nullptr;
        SDL_GPUShader* meshVertexShader = nullptr;
        SDL_GPUShader* meshFragmentShader = nullptr;
        SDL_GPUSampler* nearestSampler = nullptr;
        
        std::vector<MeshRenderCommand> renderMeshBuffer;
        Camera camera;

        SDL_GPUBuffer* meshUniformBuffer = nullptr;
        SDL_GPUShader* LoadShader(SDL_GPUDevice* device,
            const char* shaderFilename,
	        Uint32 samplerCount,
            Uint32 uniformBufferCount,
            Uint32 storageBufferCount,
            Uint32 storageTextureCount);
        bool createMeshPipeline();
        void renderMesh(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass, const MeshRenderCommand& meshCmd, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
        void updateCamera(float dt);  // optional orbit
        void init3DResources();
        void cleanup3DResources();
    };
#endif
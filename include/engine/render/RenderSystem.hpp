#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <memory>
#include "engine/render/Mesh.hpp"
#include "Camera.hpp"
#include "engine/ecs/components/Transform.hpp"
#include "engine/render/RenderTarget.hpp"
#include "engine/render/IRenderPass.hpp"
class Engine;
class MeshPass;


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
        void renderToTarget(RenderTarget& target);

        Camera& getCamera() { return camera; }
        bool SubmitMesh(Mesh* mesh, const Transform& transform);
        void SubmitDebugLine(const glm::vec3& a, const glm::vec3& b, const SDL_FColor& color);
        void ClearDebugLines(); 


        
    private:
        Engine* engine;
        SDL_GPUDevice* device;
        SDL_Window* targetWindow;
        bool resourcesInitialized = false;

        Camera camera;

        SDL_GPUBuffer* meshUniformBuffer = nullptr;


        std::unique_ptr<MeshPass> meshPass;
        std::vector<IRenderPass*> passOrder;
        void prepareAll(SDL_GPUCommandBuffer* cmd);
        void drawAll(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass);  
    };
#endif
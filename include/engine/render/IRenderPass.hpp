#ifndef I_RENDER_PASS_HPP
#define I_RENDER_PASS_HPP
#include <SDL3/SDL_gpu.h>
class Camera;
class Engine;
class IRenderPass {
public:
    virtual ~IRenderPass() = default;

    // Create pipelines/samplers/buffers. Return false on failure.
    virtual bool init(SDL_GPUDevice* device, SDL_Window* window) = 0;

    // Anything that must happen BEFORE SDL_BeginGPURenderPass:
    // copy-pass uploads (e.g. debug-line vertex data), uniform staging.
    virtual void prepare(SDL_GPUCommandBuffer* cmd) {}

    // Draw calls INSIDE the active render pass.
    virtual void draw(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass,
                      Camera& camera, Engine& engine) = 0;

    virtual void shutdown(SDL_GPUDevice* device) {}
};
#endif
#ifndef LINEPASS_HPP
#define LINEPASS_HPP
#include "engine/render/IRenderPass.hpp"
#include "engine/core/Engine.h"
#include <glm/glm.hpp>
#include <vector>

struct LineVertex {
    glm::vec3   position;   // -> TEXCOORD0 (float3)
    SDL_FColor  color;      // -> TEXCOORD1 (float4)
};

class LinePass : public IRenderPass {
public:
    bool init(SDL_GPUDevice* device, SDL_Window* window) override;
    void prepare(SDL_GPUCommandBuffer* cmd) override;   // upload vertices (pre-pass)
    void draw(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass,
              Camera& camera, Engine& engine) override;
    void shutdown(SDL_GPUDevice* device) override;

    void submit(const glm::vec3& a, const glm::vec3& b, const SDL_FColor& color);
    void clear() { vertices.clear(); dirty = true; }

private:
    bool createPipeline(SDL_Window* window);
    void ensureCapacity(size_t vertexCount);

    SDL_GPUDevice* device = nullptr;
    SDL_GPUGraphicsPipeline* pipeline = nullptr;
    SDL_GPUBuffer* vertexBuffer = nullptr;
    size_t vertexCapacity = 0;          // in vertices
    std::vector<LineVertex> vertices;
    bool dirty = false;
};
#endif
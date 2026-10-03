#ifndef MESHPASS_HPP
#define MESHPASS_HPP
#include "engine/render/IRenderPass.hpp"
#include "engine/core/Engine.h"
#include  "engine/render/Mesh.hpp"
#include "engine/ecs/components/Transform.hpp"
class MeshPass : public IRenderPass {
public:
    bool init(SDL_GPUDevice*, SDL_Window*) override;
    void draw(SDL_GPUCommandBuffer*, SDL_GPURenderPass*, Camera&, Engine&) override;
    void shutdown(SDL_GPUDevice*) override;
    bool submit(Mesh* mesh, const Transform& t);
private:
    struct MeshRenderCommand {
        Mesh* mesh;
        Transform transform;
    };
    bool createMeshPipeline(SDL_Window* window);
    void renderMesh(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass, const MeshRenderCommand& meshCmd,
                    const glm::mat4& viewMatrix, const glm::mat4& projMatrix, Camera& camera, Engine& engine);
    SDL_GPUDevice* device = nullptr;
    SDL_GPUGraphicsPipeline* meshPipeline = nullptr;
    SDL_GPUSampler* nearestSampler = nullptr;
    std::vector<MeshRenderCommand> queue;
};
#endif
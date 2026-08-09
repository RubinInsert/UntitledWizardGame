#ifndef MESH_HPP
#define MESH_HPP
#include <SDL3/SDL_gpu.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
struct MaterialInfo {
    std::string diffuseTexturePath;
    glm::vec3 diffuseColor{1.0f};  // fallback color
};
class Mesh {

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // GPU Resources
        SDL_GPUBuffer* vertexBuffer = nullptr;
        SDL_GPUBuffer* indexBuffer = nullptr;

        MaterialInfo material;
        void upload(SDL_GPUDevice* device, SDL_GPUCommandBuffer* cmd);
        void destroy(SDL_GPUDevice* device);

        static Mesh createCube();

        void calculateAABB();
        glm::vec3 aabbMin{0.0f};
        glm::vec3 aabbMax{0.0f};
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
        glm::vec3 center{0.0f};


};

#endif
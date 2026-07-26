#ifndef MESH_HPP
#define MESH_HPP
#include <SDL3/SDL_gpu.h>
#include <glm/glm.hpp>
#include <vector>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // GPU Resources
        SDL_GPUBuffer* vertexBuffer = nullptr;
        SDL_GPUBuffer* indexBuffer = nullptr;

        void upload(SDL_GPUDevice* device, SDL_GPUCommandBuffer* cmd);
        void destroy(SDL_GPUDevice* device);

        static Mesh createCube();

};

#endif
#include "engine/render/Mesh.hpp"
void Mesh::upload(SDL_GPUDevice* device, SDL_GPUCommandBuffer* cmd) {
    // --- Vertex Buffer ---
    SDL_GPUBufferCreateInfo vbInfo{};
    vbInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vbInfo.size = vertices.size() * sizeof(Vertex);
    vertexBuffer = SDL_CreateGPUBuffer(device, &vbInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = vbInfo.size;
    SDL_GPUTransferBuffer* transferBuf = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    Vertex* mapped = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuf, false);
    memcpy(mapped, vertices.data(), vbInfo.size);
    SDL_UnmapGPUTransferBuffer(device, transferBuf);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation src{ transferBuf, 0 };
    SDL_GPUBufferRegion dst{ vertexBuffer, 0, vbInfo.size };
    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);

    // --- Index Buffer ---
    SDL_GPUBufferCreateInfo ibInfo{};
    ibInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    ibInfo.size = indices.size() * sizeof(uint32_t);
    indexBuffer = SDL_CreateGPUBuffer(device, &ibInfo);

    SDL_GPUTransferBufferCreateInfo idxTransferInfo{};
    idxTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    idxTransferInfo.size = ibInfo.size;
    SDL_GPUTransferBuffer* idxTransferBuf = SDL_CreateGPUTransferBuffer(device, &idxTransferInfo);

    uint32_t* idxMapped = (uint32_t*)SDL_MapGPUTransferBuffer(device, idxTransferBuf, false);
    memcpy(idxMapped, indices.data(), ibInfo.size);
    SDL_UnmapGPUTransferBuffer(device, idxTransferBuf);

    SDL_GPUTransferBufferLocation idxSrc{ idxTransferBuf, 0 };
    SDL_GPUBufferRegion idxDst{ indexBuffer, 0, ibInfo.size };
    SDL_UploadToGPUBuffer(copyPass, &idxSrc, &idxDst, false);

    SDL_EndGPUCopyPass(copyPass);

    // Cleanup transfer buffers
    SDL_ReleaseGPUTransferBuffer(device, transferBuf);
    SDL_ReleaseGPUTransferBuffer(device, idxTransferBuf);
}
void Mesh::calculateAABB() {
    if (vertices.empty()) {
        aabbMin = glm::vec3(-0.5f);
        aabbMax = glm::vec3(0.5f);
        halfExtents = glm::vec3(0.5f);
        center = glm::vec3(0.0f);
        return;
    }

    aabbMin = glm::vec3(std::numeric_limits<float>::max());
    aabbMax = glm::vec3(-std::numeric_limits<float>::max());

    for (const auto& vertex : vertices) {
        aabbMin = glm::min(aabbMin, vertex.position);
        aabbMax = glm::max(aabbMax, vertex.position);
    }

    // Half-extents are half the total width/height/depth
    halfExtents = (aabbMax - aabbMin) * 0.5f;

    // Geometric center of the bounding box
    center = (aabbMax + aabbMin) * 0.5f;
}
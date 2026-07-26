#include "engine/render/Mesh.hpp"
Mesh Mesh::createCube() {
    Mesh mesh;
    const float h = 0.5f;

    // Front (+Z)
    Vertex front[4] = {
        {{-h, -h,  h}, { 0,  0,  1}, {0, 0}},
        {{ h, -h,  h}, { 0,  0,  1}, {1, 0}},
        {{ h,  h,  h}, { 0,  0,  1}, {1, 1}},
        {{-h,  h,  h}, { 0,  0,  1}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), front, front + 4);

    // Back (-Z)
    Vertex back[4] = {
        {{ h, -h, -h}, { 0,  0, -1}, {0, 0}},
        {{-h, -h, -h}, { 0,  0, -1}, {1, 0}},
        {{-h,  h, -h}, { 0,  0, -1}, {1, 1}},
        {{ h,  h, -h}, { 0,  0, -1}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), back, back + 4);

    // Left (-X)
    Vertex left[4] = {
        {{-h, -h, -h}, {-1,  0,  0}, {0, 0}},
        {{-h, -h,  h}, {-1,  0,  0}, {1, 0}},
        {{-h,  h,  h}, {-1,  0,  0}, {1, 1}},
        {{-h,  h, -h}, {-1,  0,  0}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), left, left + 4);

    // Right (+X)
    Vertex right[4] = {
        {{ h, -h,  h}, { 1,  0,  0}, {0, 0}},
        {{ h, -h, -h}, { 1,  0,  0}, {1, 0}},
        {{ h,  h, -h}, { 1,  0,  0}, {1, 1}},
        {{ h,  h,  h}, { 1,  0,  0}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), right, right + 4);

    // Top (+Y)
    Vertex top[4] = {
        {{-h,  h,  h}, { 0,  1,  0}, {0, 0}},
        {{ h,  h,  h}, { 0,  1,  0}, {1, 0}},
        {{ h,  h, -h}, { 0,  1,  0}, {1, 1}},
        {{-h,  h, -h}, { 0,  1,  0}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), top, top + 4);

    // Bottom (-Y)
    Vertex bottom[4] = {
        {{-h, -h, -h}, { 0, -1,  0}, {0, 0}},
        {{ h, -h, -h}, { 0, -1,  0}, {1, 0}},
        {{ h, -h,  h}, { 0, -1,  0}, {1, 1}},
        {{-h, -h,  h}, { 0, -1,  0}, {0, 1}},
    };
    mesh.vertices.insert(mesh.vertices.end(), bottom, bottom + 4);

    // Indices: 6 per face
    for (int face = 0; face < 6; ++face) {
        int base = face * 4;
        uint32_t faceIndices[] = {
            (uint32_t)base + 0, (uint32_t)base + 1, (uint32_t)base + 2,
            (uint32_t)base + 2, (uint32_t)base + 3, (uint32_t)base + 0
        };
        mesh.indices.insert(mesh.indices.end(), faceIndices, faceIndices + 6);
    }

    return mesh;
}
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
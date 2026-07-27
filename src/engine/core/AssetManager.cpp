#include "engine/core/AssetManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <stdexcept>
#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
AssetManager::AssetManager(SDL_GPUDevice* device) : device{device} {
}

AssetManager::~AssetManager() {
    if (mMasterAtlas) {
        SDL_ReleaseGPUTexture(device, mMasterAtlas);
    }
}
void AssetManager::Init() {
    // Pre-allocate the master texture canvas sheet on initialization
    SDL_GPUTextureCreateInfo atlasInfo = {};
    atlasInfo.type = SDL_GPU_TEXTURETYPE_2D;
    atlasInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    atlasInfo.width = ATLAS_SIZE;
    atlasInfo.height = ATLAS_SIZE;
    atlasInfo.layer_count_or_depth = 1;
    atlasInfo.num_levels = 1;
    atlasInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER; 

    mMasterAtlas = SDL_CreateGPUTexture(device, &atlasInfo);
    if (!mMasterAtlas) {
        throw std::runtime_error("Failed to initialize Master Texture Atlas!");
    }
}

void AssetManager::resolveRegistry(const std::string& registryPath) {
    std::string absolutePath = std::string(SDL_GetBasePath()) + registryPath;
    
    std::ifstream file(absolutePath);
    if (!file.is_open()) {
        SDL_Log("Failed to open registry: %s", absolutePath.c_str());
        return;
    }
    
    json registry;
    try {
        file >> registry;
    } catch (const json::parse_error& e) {
        SDL_Log("Registry parse error in %s: %s", registryPath.c_str(), e.what());
        return;
    }
    
    for (auto& [id, entry] : registry.items()) {
        ModelEntry model;
        model.meshPath  = entry.value("mesh", "");
        model.albedo    = entry.value("albedo", "");
        model.normal    = entry.value("normal", "");
        model.roughness = entry.value("roughness", "");
        
        mModelRegistry[id] = std::move(model);
        SDL_Log("Registered model '%s' -> %s", id.c_str(), model.meshPath.c_str());
    }
}
Mesh* AssetManager::getModel(const std::string& assetId) {
    // Check cache
    auto it = mMeshes.find(assetId);
    if (it != mMeshes.end()) return it->second.get();
    
    // Look up in registry
    auto regIt = mModelRegistry.find(assetId);
    if (regIt == mModelRegistry.end()) {
        SDL_Log("Unknown model asset: %s", assetId.c_str());
        return nullptr;
    }
    
    const ModelEntry& entry = regIt->second;
    
    // Load the mesh
    auto mesh = std::make_unique<Mesh>(loadMeshFromFile("assets/" + entry.meshPath));
    
    // Resolve the texture path from the registry
    mesh->material.diffuseTexturePath = "assets/" + entry.albedo;
    
    // Upload to GPU
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    mesh->upload(device, cmd);
    SDL_SubmitGPUCommandBuffer(cmd);
    
    auto result = mMeshes.emplace(assetId, std::move(mesh));
    return result.first->second.get();
}
Texture* AssetManager::getTexture(const std::string& filePath) {
    auto iterator = mTextures.find(filePath);
    //SDL_Log("Retrieving:", filePath.c_str());
    if(iterator != mTextures.end()) {
        //SDL_Log("Image retrieved from cache!");
        
        return iterator->second.get(); // If texture already cached; return raw pointer for observation
    } else {
        std::string absolutePath = std::string(SDL_GetBasePath()) + filePath; // SDL_GetBasePath() is cached internally in SDL3. No Freeing is required.
        SDL_Surface* surface = IMG_Load(absolutePath.c_str());
        if(!surface) { // If a texture was not found:
            // Return a pre-loaded fallback texture instead of crashing
            if(filePath == "assets/textures/missing_error.png") throw std::runtime_error(filePath + "is missing! No Fallback."); // Prevent infinite recurrsion.
            return getTexture("assets/textures/missing_error.png"); // If a texture was not found, utilise default missing error texture.
        }
        // 3. Ensure surface is in standard 32-bit RGBA format
        SDL_Surface* formattedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface); // Don't need original anymore
        if (!formattedSurface) {
            throw std::runtime_error("Failed to convert surface format: " + std::string(SDL_GetError()));
        }
        SDL_GPUTextureCreateInfo texInfo = {};
        texInfo.type = SDL_GPU_TEXTURETYPE_2D;
        texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        texInfo.width = static_cast<Uint32>(formattedSurface->w);
        texInfo.height = static_cast<Uint32>(formattedSurface->h);
        texInfo.layer_count_or_depth = 1;
        texInfo.num_levels = 1;
        texInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER; // Tells SDL we plan to sample this in a shader
        SDL_GPUTexture* rawTex = SDL_CreateGPUTexture(device, &texInfo);

        // Create a temporary Transfer (Staging) Buffer to stream bytes to VRAM
        Uint32 textureSize = formattedSurface->w * formattedSurface->h * 4;
        SDL_GPUTransferBufferCreateInfo transferInfo = {};
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferInfo.size = textureSize;

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
        // Map staging buffer memory, copy pixels from RAM, and unmap
        void* dataPtr = SDL_MapGPUTransferBuffer(device, stagingBuffer, false);
        std::memcpy(dataPtr, formattedSurface->pixels, textureSize);
        SDL_UnmapGPUTransferBuffer(device, stagingBuffer);


        // Before deleting the surface we fill our texture struct with relevant info
        std::unique_ptr<Texture> outputTexture = std::make_unique<Texture>(rawTex, formattedSurface->w, formattedSurface->h, device);
        SDL_DestroySurface(formattedSurface); // Surface data safely in staging buffer now

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    
        SDL_GPUTextureTransferInfo srcInfo = {};
        srcInfo.transfer_buffer = stagingBuffer;
        srcInfo.offset = 0;

        SDL_GPUTextureRegion dstRegion = {};
        dstRegion.texture = rawTex;
        dstRegion.w = texInfo.width;
        dstRegion.h = texInfo.height;
        dstRegion.d = 1;

        // Execute copy and immediately submit command
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
        SDL_UploadToGPUTexture(copyPass, &srcInfo, &dstRegion, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);

        // Safely release staging resources (the GPU safely references this until complete)
        SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
        auto cachedTexture = mTextures.emplace(filePath, std::move(outputTexture)); // Transfer ownership of the pointer into the map
        return cachedTexture.first->second.get(); // Return raw pointer for observation
    }
}
SpriteSheet* AssetManager::getSpriteSheet(const std::string& filePath, 
                                          float frameWidth, float frameHeight,
                                          int cols, int rows, float marginX, float marginY, float spacingX, float spacingY) {
    auto it = mSpriteSheets.find(filePath);
    if (it != mSpriteSheets.end()) {
        return it->second.get();  // Return raw observer pointer
    }
    
    // Create dynamically on the heap inside a unique_ptr
    auto spriteSheet = std::make_unique<SpriteSheet>(*this, filePath, frameWidth, frameHeight, cols, rows, marginX, marginY, spacingX, spacingY);
    
    auto cachedSpriteSheet = mSpriteSheets.emplace(filePath, std::move(spriteSheet));
    return cachedSpriteSheet.first->second.get(); // Return observable raw pointer
}

void AssetManager::setGPUDevice(SDL_GPUDevice* device) {
     this->device = device;
}

Mesh AssetManager::loadMeshFromFile(const std::string& path) {
    Assimp::Importer importer;
    
    // Import with triangulation + generate normals if missing
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs
    );
    
    if (!scene || !scene->mRootNode || !scene->mNumMeshes) {
        SDL_Log("Failed to load model: %s", importer.GetErrorString());
        return Mesh::createCube(); // fallback to cubes
    }
    
    // Take the first mesh for now (expand later)
    aiMesh* aiMesh = scene->mMeshes[0];
    Mesh mesh;
    
    // Copy vertices
    mesh.vertices.reserve(aiMesh->mNumVertices);
    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        Vertex vert;
        vert.position.x = aiMesh->mVertices[i].x;
        vert.position.y = aiMesh->mVertices[i].y;
        vert.position.z = aiMesh->mVertices[i].z;
        
        if (aiMesh->HasNormals()) {
            vert.normal.x = aiMesh->mNormals[i].x;
            vert.normal.y = aiMesh->mNormals[i].y;
            vert.normal.z = aiMesh->mNormals[i].z;
        }
        
        if (aiMesh->HasTextureCoords(0)) {
            vert.uv.x = aiMesh->mTextureCoords[0][i].x;
            vert.uv.y = aiMesh->mTextureCoords[0][i].y;
        } else {
            vert.uv = {0.0f, 0.0f};
        }
        
        mesh.vertices.push_back(vert);
    }
    
    // Copy indices
    for (unsigned int i = 0; i < aiMesh->mNumFaces; ++i) {
        aiFace& face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    // Texture loading
    if (scene->mNumMaterials > 0 && aiMesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* aiMat = scene->mMaterials[aiMesh->mMaterialIndex];
        // Try to get diffuse texture path
        aiString texPath;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            SDL_Log("Testing testing: ", texPath.C_Str());
            mesh.material.diffuseTexturePath = texPath.C_Str();
            SDL_Log("  Texture: %s", texPath.C_Str());
        }
        
        // Get fallback diffuse color
        aiColor3D color(1.0f, 1.0f, 1.0f);
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mesh.material.diffuseColor = {color.r, color.g, color.b};
    }
    SDL_Log("Loaded model: %s (%zu vertices, %zu indices)", 
            path.c_str(), mesh.vertices.size(), mesh.indices.size());
    
    return mesh;
}
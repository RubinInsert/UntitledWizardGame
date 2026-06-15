#include "engine/core/AssetManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <stdexcept>
#include <cstring>
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
Texture* AssetManager::getTexture(const std::string& filePath) {
    auto iterator = mTextures.find(filePath);
    if (iterator != mTextures.end()) {
        return iterator->second.get();  // Found Tex in cache
    }

    std::string absolutePath = std::string(SDL_GetBasePath()) + filePath;
    SDL_Surface* surface = IMG_Load(absolutePath.c_str());
    
    if (!surface) {  // Image couldnt be loaded
        if (filePath == "assets/textures/missing_error.png") { // Missing Error image couldnt be loaded
            throw std::runtime_error(filePath + " is missing! No Fallback.");
        }
        SDL_Log("Failed to load: ", filePath);
        return getTexture("assets/textures/missing_error.png"); 
    }

    SDL_Surface* formattedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32); // Ensure correct format
    SDL_DestroySurface(surface); // Destroy original surface
    if (!formattedSurface) {
        throw std::runtime_error("Failed to convert surface format: " + std::string(SDL_GetError()));
    }

    Uint32 imgW = static_cast<Uint32>(formattedSurface->w);
    Uint32 imgH = static_cast<Uint32>(formattedSurface->h);

    
    // Shelf Packing: Check if image fits horizontally on the current shelf row
    if (mCurrentX + imgW + PADDING > ATLAS_SIZE) { // If it doesnt fit
        mCurrentX = PADDING;                  // Wrap around to a brand new row
        mCurrentY += mMaxRowHeight + PADDING; // Push down past the tallest element of old row
        mMaxRowHeight = 0;                    // Reset height tracker for the new shelf row
    }

    // Check if we ran out of vertical space completely on our canvas
    if (mCurrentY + imgH + PADDING > ATLAS_SIZE) {
        SDL_DestroySurface(formattedSurface);
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Master Texture Atlas is full! Increase ATLAS_SIZE.");
        return getTexture("assets/textures/missing_error.png");
    }

    // Save assigned coordinates for packing location targets
    Uint32 packedX = mCurrentX;
    Uint32 packedY = mCurrentY;

    // Advance horizontal line position & keep track of row height
    mCurrentX += imgW + PADDING;
    if (imgH > mMaxRowHeight) {
        mMaxRowHeight = imgH;
    }

    // Normalized UV coordinate translations (0.0f -> 1.0f range mapped context)
    float uMin   = static_cast<float>(packedX) / static_cast<float>(ATLAS_SIZE);
    float vMin   = static_cast<float>(packedY) / static_cast<float>(ATLAS_SIZE);
    float uWidth = static_cast<float>(imgW)    / static_cast<float>(ATLAS_SIZE);
    float vHeight= static_cast<float>(imgH)    / static_cast<float>(ATLAS_SIZE);

    // Stream pixel bytes via Staging Buffer allocations
    Uint32 textureSize = imgW * imgH * 4;
    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = textureSize;

    SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
    void* dataPtr = SDL_MapGPUTransferBuffer(device, stagingBuffer, false);
    std::memcpy(dataPtr, formattedSurface->pixels, textureSize); // Copy the surface pixels to the transfer staging area in memory
    SDL_UnmapGPUTransferBuffer(device, stagingBuffer); // Pass ownership back to GPU

    // Assemble individual details object wrapping our single master atlas handler 
    auto outputTexture = std::make_unique<Texture>(mMasterAtlas, imgW, imgH, uMin, vMin, uWidth, vHeight);
    SDL_DestroySurface(formattedSurface); 

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    
    SDL_GPUTextureTransferInfo srcInfo = {};
    srcInfo.transfer_buffer = stagingBuffer;
    srcInfo.offset = 0;

    // Direct upload targeting the designated sub-rectangle section coordinates
    SDL_GPUTextureRegion dstRegion = {};
    dstRegion.texture = mMasterAtlas;
    dstRegion.x = packedX;
    dstRegion.y = packedY;
    dstRegion.w = imgW;
    dstRegion.h = imgH;
    dstRegion.d = 1;

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    SDL_UploadToGPUTexture(copyPass, &srcInfo, &dstRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);

    SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
    
    auto cachedTexture = mTextures.emplace(filePath, std::move(outputTexture)); 
    return cachedTexture.first->second.get(); 
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
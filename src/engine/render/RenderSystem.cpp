#include "engine/render/RenderSystem.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>
RenderSystem::RenderSystem() {
    // Leave empty for now or initialize pointers to nullptr
}
RenderSystem::RenderSystem(SDL_GPUDevice* device) {
    this->device = device;
}

RenderSystem::~RenderSystem() {
    if (spritePipeline) SDL_ReleaseGPUGraphicsPipeline(device, spritePipeline);
    if (defaultSampler) SDL_ReleaseGPUSampler(device, defaultSampler);
    if (spriteTransferBuffer) SDL_ReleaseGPUTransferBuffer(device, spriteTransferBuffer);
    if (spriteStorageBuffer) SDL_ReleaseGPUBuffer(device, spriteStorageBuffer);
}
void RenderSystem::setGPUDevice(SDL_GPUDevice* device) { this->device = device; }
void RenderSystem::setTargetWindow(SDL_Window* window) { targetWindow = window; }
void RenderSystem::draw(const RenderSprite& sprite) { spriteQueue.push(sprite); }
void RenderSystem::initResources(int width, int height) {
	if(!createShaders()) return;
	if(!createGraphicsPipeline()) return;
	
	// Point Clamp (Nearest Neighbour Sampler)
		SDL_GPUSamplerCreateInfo samplerInfo = {};
		samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
		samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
		samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
		samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		defaultSampler = SDL_CreateGPUSampler(device, &samplerInfo);
	// Create Transfer Upload buffer for structured data instances
		SDL_GPUTransferBufferCreateInfo transferInfo = {};
		transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		transferInfo.size = MAX_SPRITES * sizeof(SpriteInstance);
		spriteTransferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
	// Create GPU read Storage Buffer 
		SDL_GPUBufferCreateInfo bufferInfo = {};
		bufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
		bufferInfo.size = MAX_SPRITES * sizeof(SpriteInstance);
		spriteStorageBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    resourcesInitialized = true;
}
bool RenderSystem::createShaders() {
    // Vertex shader needs 1 uniform buffer for the projection matrix
    vertexShader = LoadShader(device, "sprite.vert", 0, 1, 1, 0);
    if (!vertexShader) return false;

    // Fragment shader uses 1 sampler for the texture.
    fragmentShader = LoadShader(device, "sprite.frag", 1, 0, 0, 0);
    if (!fragmentShader) return false;

    return true;
}

void RenderSystem::render() {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if(!cmd) return;

    // Run render passes
    runHeightMapPass(cmd);
    runShadowPass(cmd);

    runAlbedoPass(cmd);
	while(!spriteQueue.empty()) spriteQueue.pop();
    SDL_SubmitGPUCommandBuffer(cmd);
}

void RenderSystem::processSpriteQueue(SDL_GPUCommandBuffer* cmd) {
    if (spriteQueue.empty()) return;

    Uint32 spriteCount = SDL_min((Uint32)spriteQueue.size(), MAX_SPRITES);

    // Map transfer memory zone
    SpriteInstance* dataPtr = (SpriteInstance*)SDL_MapGPUTransferBuffer(device, spriteTransferBuffer, true);
    
    // Copy queue details sequentially into the staging region
    std::queue<RenderSprite> tempQueue = spriteQueue;
    for (Uint32 i = 0; i < spriteCount; ++i) {
        RenderSprite sprite = tempQueue.front();
        tempQueue.pop();

        dataPtr[i].x = sprite.destRect.x;
        dataPtr[i].y = sprite.destRect.y;
        dataPtr[i].z = 0.0f;
        dataPtr[i].rotation = 0.0f; // Assign rotation parameters (future implementation))
        dataPtr[i].w = sprite.destRect.w;
        dataPtr[i].h = sprite.destRect.h;
        
        // Match source texture coordinates maps normalized (0.0 -> 1.0)
        // Get the pixel dimensions of the original spritesheet
		float sheetW = static_cast<float>(sprite.texture->width);
		float sheetH = static_cast<float>(sprite.texture->height);

		// Normalize the local frame boundaries relative to its own sheet (0.0 to 1.0)
		float localU = sprite.sourceRect.x / sheetW;
		float localV = sprite.sourceRect.y / sheetH;
		float localW = sprite.sourceRect.w / sheetW;
		float localH = sprite.sourceRect.h / sheetH;

		// Scale and shift those local coordinates into global Master Atlas UV space
		dataPtr[i].tex_u = sprite.texture->uMin + (localU * sprite.texture->uWidth);
		dataPtr[i].tex_v = sprite.texture->vMin + (localV * sprite.texture->vHeight);
		dataPtr[i].tex_w = localW * sprite.texture->uWidth;
		dataPtr[i].tex_h = localH * sprite.texture->vHeight;
		

		// Assign color values (future implementation)
        dataPtr[i].r = 1.0f;
        dataPtr[i].g = 1.0f;
        dataPtr[i].b = 1.0f;
        dataPtr[i].a = 1.0f;
    }

    SDL_UnmapGPUTransferBuffer(device, spriteTransferBuffer); // Ownership back to GPU

    // Create GPU hardware stream pipeline copy allocations
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTransferBufferLocation srcLocation = { spriteTransferBuffer, 0 };
    SDL_GPUBufferRegion dstRegion = { spriteStorageBuffer, 0, spriteCount * sizeof(SpriteInstance) };
    
    SDL_UploadToGPUBuffer(copyPass, &srcLocation, &dstRegion, true);
    SDL_EndGPUCopyPass(copyPass);
}

// Maps a rectangle of pixel space directly to the normalized GPU screen space (0.0 -> 1.0f)
// Generated with DeepSeek-V3
Matrix4x4 RenderSystem::CreateOrthoProjection(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Matrix4x4 result = {0};
    result.m[0]  = 2.0f / (right - left);
    result.m[5]  = 2.0f / (top - bottom);
    result.m[10] = 1.0f / (farZ - nearZ);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -nearZ / (farZ - nearZ);
    result.m[15] = 1.0f;
    return result;
}
void RenderSystem::runHeightMapPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runShadowPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runAlbedoPass(SDL_GPUCommandBuffer* cmd) {
    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if(SDL_WaitAndAcquireGPUSwapchainTexture (cmd, targetWindow, &swapchainTex, &w, &h)) {
        processSpriteQueue(cmd);
		// Define the color target info
        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = swapchainTex;
        colorTargetInfo.clear_color = { 0.39f, 0.58f, 0.93f, 1.0f };; // Black background
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    //     // Begin the pass that draws to the screen
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, nullptr);
        if(renderPass) {
			SDL_BindGPUGraphicsPipeline(renderPass, spritePipeline);
			
			// Bind the active storage buffer to slot index 0
			SDL_BindGPUVertexStorageBuffers(renderPass, 0, &spriteStorageBuffer, 1);
			
			// Bind texture 
			SDL_GPUTextureSamplerBinding samplerBinding = {};
            samplerBinding.texture = spriteQueue.front().texture->get();
            samplerBinding.sampler = defaultSampler;
            SDL_BindGPUFragmentSamplers(renderPass, 0, &samplerBinding, 1);

			// Push camera ortho matrix definitions
			Matrix4x4 projection = CreateOrthoProjection(0.0f, (float)w, (float)h, 0.0f, 0.0f, 1.0f);
			SDL_PushGPUVertexUniformData(cmd, 0, &projection, sizeof(projection));
			
			SDL_DrawGPUPrimitives(renderPass, (Uint32)spriteQueue.size() * 6, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        
    }
}
// To be moved to Asset Manager
// https://github.com/TheSpydog/SDL_gpu_examples/blob/main/Examples/Common.c
SDL_GPUShader* RenderSystem::LoadShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return NULL;
	}

	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/SPIRV/%s.spv", SDL_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/MSL/%s.msl", SDL_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/DXIL/%s.dxil", SDL_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return NULL;
	}
	SDL_Log("Attempting to load shader from: %s", fullPath);
	size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == NULL)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return NULL;
	}
	SDL_Log("Successfully loaded %zu bytes", codeSize);
    // Altered from original: to match C++ initialization convention
	SDL_GPUShaderCreateInfo shaderInfo = {
		.code_size = codeSize,
		.code = (const Uint8*)code,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_storage_textures = storageTextureCount,
		.num_storage_buffers = storageBufferCount,
		.num_uniform_buffers = uniformBufferCount
	};
	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
	if (shader == NULL)
	{
		SDL_Log("Failed to create shader!");
		SDL_free(code);
		return NULL;
	}

	SDL_free(code);
	return shader;
}

bool RenderSystem::createGraphicsPipeline() {
	SDL_Log("=== createGraphicsPipeline START ===");
    SDL_Log("Device: %p", device);
    SDL_Log("Target window: %p", targetWindow);
	// SDL_GPUVertexBufferDescription vertBufferDesc{
	// 								0,
	// 								sizeof(PositionTextureVertex),
	// 								SDL_GPU_VERTEXINPUTRATE_VERTEX,
	// 								0
	// 							};
	// SDL_GPUVertexAttribute vertexAttributes [3] = {
	// 							{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(PositionTextureVertex, pos)},
	// 							{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(PositionTextureVertex, uv)},
	// 							{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(PositionTextureVertex, color)},
	// 						};
	// SDL_GPUVertexInputState vertexInputState{
	// 							&vertBufferDesc,
	// 							1,
	// 							vertexAttributes,
	// 							3,
	// 						};
	SDL_GPURasterizerState rasterizerState{
								SDL_GPU_FILLMODE_FILL,
								SDL_GPU_CULLMODE_NONE,
								SDL_GPU_FRONTFACE_CLOCKWISE,
								0,
								0,
								0,
								false,
								false,
								0,
								0
							};
	SDL_GPUMultisampleState multiSampleState{
								SDL_GPU_SAMPLECOUNT_1,
								0,
								false,
								false,
								0,
								0
							};
	SDL_GPUStencilOpState stencilState{
							SDL_GPU_STENCILOP_KEEP,      // Keep existing stencil value on fail
							SDL_GPU_STENCILOP_KEEP,      // Keep on pass
							SDL_GPU_STENCILOP_KEEP, // Keep on depth fail
							SDL_GPU_COMPAREOP_ALWAYS   // Always pass stencil test
						};
	SDL_GPUDepthStencilState depthStencilState{
								SDL_GPU_COMPAREOP_ALWAYS,
								stencilState,
								stencilState,
								0xFF,
								0xFF,
								false,
								false,
								false,
								0,
								0,
								0
							};
	SDL_GPUColorTargetBlendState blendState {
		SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		SDL_GPU_BLENDOP_ADD,
		SDL_GPU_BLENDFACTOR_ONE,
		SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		SDL_GPU_BLENDOP_ADD,
		SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,
		true,
		true,
		0,
		0
	};
	SDL_GPUColorTargetDescription colorTargetDesc {
									SDL_GetGPUSwapchainTextureFormat(device, targetWindow), // query the device and window format dynamically
									blendState
								};
	
	SDL_GPUGraphicsPipelineTargetInfo targetInfo{
										&colorTargetDesc,
										1,
										SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
										false,
										0,
										0,
										0	
									};
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{
										vertexShader,
										fragmentShader,
										{}, // No Vertex Input because the vert shader pulls directly from buffer
										SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
										rasterizerState,
										multiSampleState,
										depthStencilState,
										targetInfo,
										0
									};

    spritePipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (!spritePipeline) {
        SDL_Log("Failed to create graphics pipeline: %s", SDL_GetError());
        return false;
    }
	SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);
    return true;
}
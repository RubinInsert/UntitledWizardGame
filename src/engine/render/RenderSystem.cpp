#include "engine/render/RenderSystem.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>
RenderSystem::RenderSystem() {
    // Leave empty for now or initialize pointers to nullptr
}
RenderSystem::RenderSystem(SDL_GPUDevice* device) {
    this->device = device;
}

RenderSystem::~RenderSystem() {
    // Cleanup code later
}

void RenderSystem::initResources(int width, int height) {
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
	textureInfo.num_levels = 1;
    heightBuffer = SDL_CreateGPUTexture(device, &textureInfo);
    shadowBuffer = SDL_CreateGPUTexture(device, &textureInfo);
    
	if(createShaders()) {
		if(createGraphicsPipeline()) {
			// Point Clamp
			SDL_GPUSamplerCreateInfo samplerInfo = {};
			samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
			samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
			samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
			samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
			samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
			samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
			defaultSampler = SDL_CreateGPUSampler(device, &samplerInfo);
	
			SDL_GPUBufferCreateInfo vertexBufferInfo = {};
			vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
			vertexBufferInfo.size = sizeof(PositionTextureVertex) * 4;
			vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo);
			SDL_SetGPUBufferName(device, vertexBuffer, "defaultVertexBuffer");
			
			SDL_GPUBufferCreateInfo indexBufferInfo = {};
			indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
			indexBufferInfo.size = sizeof(Uint16) * 6;
			indexBuffer = SDL_CreateGPUBuffer(device, &indexBufferInfo);

			// 1. Create a Transfer Buffer for the staging process
SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
transferBufferInfo.size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6);
SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);

// 2. Map the memory and copy CPU data into it
void* mapPtr = SDL_MapGPUTransferBuffer(device, transferBuffer, false);

// Define standard quad vertices: NDC space (-1 to 1) or normalized screen space
PositionTextureVertex* vertices = (PositionTextureVertex*)mapPtr;
vertices[0] = { {-0.5f,  0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }; // Top Left
vertices[1] = { { 0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }; // Top Right
vertices[2] = { { 0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }; // Bottom Right
vertices[3] = { {-0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }; // Bottom Left

// Define indices right after the vertices in the same map pointer
Uint16* indices = (Uint16*)((Uint8*)mapPtr + (sizeof(PositionTextureVertex) * 4));
indices[0] = 0; indices[1] = 1; indices[2] = 2;
indices[3] = 0; indices[4] = 2; indices[5] = 3;

SDL_UnmapGPUTransferBuffer(device, transferBuffer);

// 3. Command buffer upload copy
SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(device);
SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

SDL_GPUTransferBufferLocation srcLocation = { transferBuffer, 0 };
SDL_GPUBufferRegion dstRegion = { vertexBuffer, 0, sizeof(PositionTextureVertex) * 4 };
SDL_UploadToGPUBuffer(copyPass, &srcLocation, &dstRegion, false);

// Offset copy location for indices
srcLocation.offset = sizeof(PositionTextureVertex) * 4;
dstRegion.buffer = indexBuffer;
dstRegion.size = sizeof(Uint16) * 6;
SDL_UploadToGPUBuffer(copyPass, &srcLocation, &dstRegion, false);

SDL_EndGPUCopyPass(copyPass);
SDL_SubmitGPUCommandBuffer(uploadCmd);

// Clean up the staging transfer buffer
SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
		}
	}
    resourcesInitialized = true;
}
bool RenderSystem::createShaders() {
    // Vertex shader needs 1 uniform buffer for the projection matrix
    vertexShader = LoadShader(device, "sprite.vert", 0, 2, 0, 0);
    if (!vertexShader) return false;

    // Fragment shader uses 1 sampler for the texture.
    fragmentShader = LoadShader(device, "sprite.frag", 1, 0, 0, 0);
    if (!fragmentShader) return false;

    return true;
}
void RenderSystem::setGPUDevice(SDL_GPUDevice* device) {
    this->device = device;
}
void RenderSystem::setTargetWindow(SDL_Window* window) {
    targetWindow = window;
}
void RenderSystem::render() {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if(!cmd) return;

    // Run render passes
    runHeightMapPass(cmd);
    runShadowPass(cmd);

    runAlbedoPass(cmd);

    SDL_SubmitGPUCommandBuffer(cmd);
}
void RenderSystem::draw(const RenderSprite& sprite) {
    spriteQueue.push(sprite);
}
void RenderSystem::processSpriteQueue(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* renderPass) {
    // For now, just log that we're processing
    SDL_Log("Processing %zu sprites", spriteQueue.size());
    
    while (!spriteQueue.empty()) {
        RenderSprite sprite = spriteQueue.front();
        spriteQueue.pop();
        paintSprite(cmd, renderPass, sprite);
        // TODO: Draw the sprite
        //SDL_Log("Drawing sprite with texture %p at position (%f, %f)", 
                //sprite.texture, sprite.destRect.x, sprite.destRect.y);
    }
}

void RenderSystem::paintSprite(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* renderPass, const RenderSprite& sprite) {
	Matrix4x4 modelMatrix = CalculateModelMatrix(sprite.destRect.x, sprite.destRect.y, sprite.destRect.w, sprite.destRect.h);
	SDL_PushGPUVertexUniformData(cmd, 1, &modelMatrix, sizeof(modelMatrix));
	
	SDL_GPUTextureSamplerBinding binding = {};
    binding.texture = sprite.texture->get();
    binding.sampler = defaultSampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &binding, 1);
	struct MyShaderUniforms {
        float matrix[16]; // Projection/View/Model combined
    } uniforms;

    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);


}
void RenderSystem::runHeightMapPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runShadowPass(SDL_GPUCommandBuffer* cmd) {}
void RenderSystem::runAlbedoPass(SDL_GPUCommandBuffer* cmd) {
    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if(SDL_WaitAndAcquireGPUSwapchainTexture (cmd, targetWindow, &swapchainTex, &w, &h)) {
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
            Matrix4x4 projection = CreateOrthoProjection(0.0f, (float)w, (float)h, 0.0f, 0.0f, 1.0f);
			SDL_PushGPUVertexUniformData(cmd, 0, &projection, sizeof(projection));
            // Fix: Create binding variables
            SDL_GPUBufferBinding vertexBinding = {};
            vertexBinding.buffer = vertexBuffer;
            vertexBinding.offset = 0;
            SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
            
            SDL_GPUBufferBinding indexBinding = {};
            indexBinding.buffer = indexBuffer;
            indexBinding.offset = 0;
            SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
            processSpriteQueue(cmd, renderPass);

            SDL_EndGPURenderPass(renderPass);
        }

        
    }
}
Matrix4x4 RenderSystem::CreateOrthoProjection(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Matrix4x4 result = {0};

    // Column-major layout
    result.m[0]  = 2.0f / (right - left);
    result.m[5]  = 2.0f / (top - bottom);
    result.m[10] = 1.0f / (farZ - nearZ); // Note: SDL3 GPU uses [0, 1] depth range
    
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -nearZ / (farZ - nearZ);
    result.m[15] = 1.0f;

    return result;
}
Matrix4x4 RenderSystem::CalculateModelMatrix(float x, float y, float width, float height) {
    Matrix4x4 result = {0};
    
    // Start with identity matrix
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    
    // Scale: from unit quad to desired size
    // Your quad vertices are [-0.5, 0.5] range, so width/height gives exact size
    result.m[0] = width;   // Scale X
    result.m[5] = height;  // Scale Y
    
    // Translate to world position
    result.m[12] = x;      // Translate X
    result.m[13] = y;      // Translate Y
    
    return result;
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
	SDL_GPUVertexBufferDescription vertBufferDesc{
									0,
									sizeof(PositionTextureVertex),
									SDL_GPU_VERTEXINPUTRATE_VERTEX,
									0
								};
	SDL_GPUVertexAttribute vertexAttributes [3] = {
								{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(PositionTextureVertex, pos)},
								{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(PositionTextureVertex, uv)},
								{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(PositionTextureVertex, color)},
							};
	SDL_GPUVertexInputState vertexInputState{
								&vertBufferDesc,
								1,
								vertexAttributes,
								3,
							};
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
								SDL_GPU_COMPAREOP_LESS,
								stencilState,
								stencilState,
								0xFF,
								0xFF,
								true,
								true,
								false,
								0,
								0,
								0
							};
	SDL_GPUColorTargetBlendState blendState {
		SDL_GPU_BLENDFACTOR_ONE,
		SDL_GPU_BLENDFACTOR_ZERO,
		SDL_GPU_BLENDOP_ADD,
		SDL_GPU_BLENDFACTOR_ONE,
		SDL_GPU_BLENDFACTOR_ZERO,
		SDL_GPU_BLENDOP_ADD,
		SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,
		false,
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
										vertexInputState,
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
    return true;
}
#include "engine/render/RenderSystem.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>
RenderSystem::RenderSystem() {
    // Leave empty for now or initialize pointers to nullptr
}
RenderSystem::RenderSystem(SDL_GPUDevice* device) {
    this->device = device;
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::setGPUDevice(SDL_GPUDevice* device) { this->device = device; }
void RenderSystem::setTargetWindow(SDL_Window* window) { targetWindow = window; }

void RenderSystem::initResources(int width, int height) {
    SDL_GPUCommandBuffer* setupCmd = SDL_AcquireGPUCommandBuffer(device);
    testCube = Mesh::createCube();
    testCube.upload(device, setupCmd);
    SDL_SubmitGPUCommandBuffer(setupCmd);
    if (!createMeshPipeline()) return;
    resourcesInitialized = true;
}

void RenderSystem::render() {
	if (!resourcesInitialized) return;
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if(!cmd) return;
    renderCube(cmd);
    SDL_SubmitGPUCommandBuffer(cmd);
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

bool RenderSystem::createMeshPipeline() {
    SDL_Log("=== createMeshPipeline START ===");

    SDL_GPUShader* vertShader = LoadShader(device, "cube.vert", 0, 1, 0, 0);
    if (!vertShader) return false;
    SDL_GPUShader* fragShader = LoadShader(device, "cube.frag", 0, 0, 0, 0);
    if (!fragShader) return false;

    // Vertex buffer description — one stream of interleaved Vertex structs
    SDL_GPUVertexBufferDescription vertBufferDesc{
        0,                           // slot
        sizeof(Vertex),              // pitch (32 bytes: pos3 + normal3 + uv2)
        SDL_GPU_VERTEXINPUTRATE_VERTEX,  // input_rate
        0                            // instance_step_rate
    };

    // Vertex attributes — 3 attributes matching HLSL TEXCOORD0/1/2
    SDL_GPUVertexAttribute vertexAttributes[3] = {
        {0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, position)},  // TEXCOORD0
        {1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, normal)},    // TEXCOORD1
        {2, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Vertex, uv)}         // TEXCOORD2
    };

    SDL_GPUVertexInputState vertexInputState{
        &vertBufferDesc,
        1,
        vertexAttributes,
        3
    };

    // Rasterizer — backface culling for solid 3D
    SDL_GPURasterizerState rasterizerState{
        SDL_GPU_FILLMODE_FILL,
        SDL_GPU_CULLMODE_BACK,             // was NONE for sprites
        SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE, // was CLOCKWISE for sprites
        0, 0, 0,
        false, false,
        0, 0
    };

    // Multisample
    SDL_GPUMultisampleState multiSampleState{
        SDL_GPU_SAMPLECOUNT_1,
        0, false, false, 0, 0
    };

    // Stencil (unused)
    SDL_GPUStencilOpState stencilState{
        SDL_GPU_STENCILOP_KEEP,
        SDL_GPU_STENCILOP_KEEP,
        SDL_GPU_STENCILOP_KEEP,
        SDL_GPU_COMPAREOP_ALWAYS
    };

    // Depth state — REAL depth testing enabled
    SDL_GPUDepthStencilState depthStencilState{
        SDL_GPU_COMPAREOP_LESS,  // was ALWAYS for sprites
        stencilState,
        stencilState,
        0xFF,
        0xFF,
        true,   // depth_test_enabled  (was false)
        true,   // depth_write_enabled (was false)
        false,  // stencil_test_enabled
        0, 0, 0
    };

    // Blend — opaque (no blending needed for solid cube)
    SDL_GPUColorTargetBlendState blendState{
        SDL_GPU_BLENDFACTOR_ONE,               // src_color_blendfactor
        SDL_GPU_BLENDFACTOR_ZERO,              // dst_color_blendfactor
        SDL_GPU_BLENDOP_ADD,                   // color_blend_op
        SDL_GPU_BLENDFACTOR_ONE,               // src_alpha_blendfactor
        SDL_GPU_BLENDFACTOR_ZERO,              // dst_alpha_blendfactor
        SDL_GPU_BLENDOP_ADD,                   // alpha_blend_op
        SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
            SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,
        false,  // enable_blend  (opaque, no blending)
        false,  // enable_color_write_mask (wait, check this)
        0, 0
    };

    SDL_GPUColorTargetDescription colorTargetDesc{
        SDL_GetGPUSwapchainTextureFormat(device, targetWindow),
        blendState
    };

    SDL_GPUGraphicsPipelineTargetInfo targetInfo{
        &colorTargetDesc,
        1,
        SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
        true,   // has_depth_stencil_target (need this for depth!)
        0, 0, 0
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{
        vertShader,
        fragShader,
        vertexInputState,                    // was {} for sprites
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        rasterizerState,
        multiSampleState,
        depthStencilState,
        targetInfo,
        0
    };

    meshPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (!meshPipeline) {
        SDL_Log("Failed to create mesh pipeline: %s", SDL_GetError());
        SDL_ReleaseGPUShader(device, vertShader);
        SDL_ReleaseGPUShader(device, fragShader);
        return false;
    }

    SDL_ReleaseGPUShader(device, vertShader);
    SDL_ReleaseGPUShader(device, fragShader);
    SDL_Log("=== createMeshPipeline SUCCESS ===");
    return true;
}
void RenderSystem::renderCube(SDL_GPUCommandBuffer* cmd) {
    SDL_GPUTexture* swapchainTex;
    Uint32 w, h;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, targetWindow, &swapchainTex, &w, &h))
        return;
	camera.aspectRatio = (float)w / (float)h;
    // Create depth texture if needed
    static SDL_GPUTexture* depthTexture = nullptr;
    static Uint32 depthW = 0, depthH = 0;
    if (!depthTexture || depthW != w || depthH != h) {
        if (depthTexture) SDL_ReleaseGPUTexture(device, depthTexture);
        SDL_GPUTextureCreateInfo depthInfo{};
        depthInfo.type = SDL_GPU_TEXTURETYPE_2D;
        depthInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
        depthInfo.width = w;
        depthInfo.height = h;
        depthInfo.layer_count_or_depth = 1;
        depthInfo.num_levels = 1;
        depthInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        depthTexture = SDL_CreateGPUTexture(device, &depthInfo);
        depthW = w; depthH = h;
    }

    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTex;
    colorTarget.clear_color = {0.1f, 0.1f, 0.2f, 1.0f};
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = depthTexture;
    depthTarget.clear_depth = 1.0f;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, &depthTarget);
    if (!pass) return;

	if (!meshPipeline) {
		SDL_Log("ERROR: meshPipeline is null! Pipeline was not created.");
		SDL_EndGPURenderPass(pass);
		return;
	}
    SDL_BindGPUGraphicsPipeline(pass, meshPipeline);

    // Push MVP uniforms (matches your HLSL: register(b0, space1))
    struct { glm::mat4 model; glm::mat4 view; glm::mat4 projection; } uniforms;
    uniforms.model = glm::mat4(1.0f);
    uniforms.view = camera.getViewMatrix();
    uniforms.projection = camera.getProjectionMatrix();
    SDL_PushGPUVertexUniformData(cmd, 0, &uniforms, sizeof(uniforms));

    // Bind vertex & index buffers
    SDL_GPUBufferBinding vbBind{ testCube.vertexBuffer, 0 };
    SDL_BindGPUVertexBuffers(pass, 0, &vbBind, 1);
	SDL_GPUBufferBinding ibBind{ testCube.indexBuffer, 0 };
	SDL_BindGPUIndexBuffer(pass, &ibBind, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    SDL_DrawGPUIndexedPrimitives(pass, (Uint32)testCube.indices.size(), 1, 0, 0, 0);
    SDL_EndGPURenderPass(pass);
}

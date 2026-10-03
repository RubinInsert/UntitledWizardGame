#include "engine/render/pipelines/MeshPass.hpp"
#include "engine/render/ShaderLoader.hpp"
#include "engine/ecs/components/MeshComponent.hpp"
#include "engine/ecs/components/Transform.hpp"
bool MeshPass::init(SDL_GPUDevice* dev, SDL_Window* window) {
    device = dev;
    if (!createMeshPipeline(window)) return false;
    SDL_GPUSamplerCreateInfo s{}; s.min_filter = s.mag_filter = SDL_GPU_FILTER_NEAREST;
    s.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    nearestSampler = SDL_CreateGPUSampler(device, &s);
    return nearestSampler != nullptr;
}
bool MeshPass::submit(Mesh* mesh, const Transform& t) { queue.push_back({mesh, t}); return true; }
void MeshPass::draw(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass, Camera& camera, Engine& engine) {
    if (!meshPipeline) return;
    SDL_BindGPUGraphicsPipeline(pass, meshPipeline);
    auto view = engine.getRegistry().view<MeshComponent, Transform>();   // <- this loop moves out of internalRender
    for (auto e : view)
        renderMesh(cmd, pass, {view.get<MeshComponent>(e).mesh, view.get<Transform>(e)},
                   camera.getViewMatrix(), camera.getProjectionMatrix(), camera, engine);
    for (const auto& c : queue)
        renderMesh(cmd, pass, c, camera.getViewMatrix(), camera.getProjectionMatrix(), camera, engine);
    queue.clear();
}
void MeshPass::shutdown(SDL_GPUDevice* dev) {
    if (meshPipeline) SDL_ReleaseGPUGraphicsPipeline(dev, meshPipeline);
    if (nearestSampler) SDL_ReleaseGPUSampler(dev, nearestSampler);
}
bool MeshPass::createMeshPipeline(SDL_Window* window) {
    SDL_Log("=== createMeshPipeline START ===");

    SDL_GPUShader* vertShader = ShaderLoader::Load(device, "cube.vert", 0, 1, 0, 0);
    if (!vertShader) return false;
    SDL_GPUShader* fragShader = ShaderLoader::Load(device, "cube.frag", 1, 1, 0, 0);
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
        SDL_GetGPUSwapchainTextureFormat(device, window),
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
void MeshPass::renderMesh(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass, const MeshRenderCommand& meshCmd, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, Camera& camera, Engine& engine) {
    // Push MVP uniforms (matches your HLSL: register(b0, space1))
    struct { glm::mat4 model; glm::mat4 view; glm::mat4 projection; } uniforms;
    // uniforms.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), meshCmd.transform.position);
    glm::mat4 rotation    = glm::mat4_cast(meshCmd.transform.rotation); // Converts glm::quat to glm::mat4
    glm::mat4 scale       = glm::scale(glm::mat4(1.0f), meshCmd.transform.scale);

    uniforms.model      = translation * rotation * scale;
    uniforms.view       = viewMatrix;
    uniforms.projection = projMatrix;
    SDL_PushGPUVertexUniformData(cmd, 0, &uniforms, sizeof(uniforms));

	struct LightGPU {
    glm::vec4 direction;
    glm::vec4 color;
    glm::vec4 ambient;
    glm::vec4 cameraPos;
	} lightData;
	lightData.direction = glm::vec4(glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)), 0.0f);
	lightData.color     = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	lightData.ambient   = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f);
	lightData.cameraPos = glm::vec4(camera.position, 1.0f);
	SDL_PushGPUFragmentUniformData(cmd, 0, &lightData, sizeof(lightData));
    // Bind vertex & index buffers
    SDL_GPUBufferBinding vbBind{ meshCmd.mesh->vertexBuffer, 0 };
    SDL_BindGPUVertexBuffers(pass, 0, &vbBind, 1);
	SDL_GPUBufferBinding ibBind{ meshCmd.mesh->indexBuffer, 0 };
	SDL_BindGPUIndexBuffer(pass, &ibBind, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    Texture* tex = engine.getAssetManager().getTexture(meshCmd.mesh->material.diffuseTexturePath);
    SDL_GPUTextureSamplerBinding texBind{ tex->get(), nearestSampler };
    SDL_BindGPUFragmentSamplers(pass, 0, &texBind, 1);

    SDL_DrawGPUIndexedPrimitives(pass, (Uint32)meshCmd.mesh->indices.size(), 1, 0, 0, 0);
    
}
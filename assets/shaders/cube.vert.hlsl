// Register spaces are significant for resource binding in DirectX 12, ensuring no conflicts with other resources.
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
ConstantBuffer<UniformBuffer> ubo : register(b0, space1);

struct VertexInput {
    float3 position : TEXCOORD0;
    float3 normal   : TEXCOORD1;
    float2 uv       : TEXCOORD2;
};
struct VertexOutput {
    float4 position : SV_POSITION;
    float3 normal   : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};


// Index mapping to generate a standard quad from 6 vertices
static const uint triangleIndices[6] = {0, 1, 2, 3, 2, 1};

// Standard Top-Left anchored quad coordinates (0 to 1 space)
static const float2 vertexPos[4] = {
    {0.0f, 0.0f}, // Top Left
    {1.0f, 0.0f}, // Top Right
    {0.0f, 1.0f}, // Bottom Left
    {1.0f, 1.0f}  // Bottom Right
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    float4 worldPos = mul(ubo.model, float4(input.position, 1.0));
    output.position = mul(ubo.projection, mul(ubo.view, worldPos));
    
    // Transform normal (assumes no non-uniform scale)
    output.normal = mul((float3x3)ubo.model, input.normal);
    output.worldPos = worldPos.xyz;
    
    return output;
}
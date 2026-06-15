struct SpriteData
{
    float3 Position;
    float Rotation;
    float2 Scale;      // Maps to w and h from C++ struct
    float2 Padding;    // Maps to padding_a and padding_b
    float TexU, TexV, TexW, TexH;
    float4 Color;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
};

// Register spaces are significant for resource binding in DirectX 12, ensuring no conflicts with other resources.
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader

// Bound in slot 0 via SDL_BindGPUVertexStorageBuffers
StructuredBuffer<SpriteData> DataBuffer : register(t0, space0);

// Bound in slot 0 via SDL_PushGPUVertexUniformData
cbuffer UniformBlock : register(b0, space1)
{
    float4x4 ViewProjectionMatrix : packoffset(c0);
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

Output main(uint id : SV_VertexID)
{
    uint spriteIndex = id / 6;
    uint vert = triangleIndices[id % 6];
    SpriteData sprite = DataBuffer[spriteIndex];

    // Map out UV bounding boxes explicitly for the 4 corners
    float2 texcoord[4] = {
        {sprite.TexU,               sprite.TexV},
        {sprite.TexU + sprite.TexW, sprite.TexV},
        {sprite.TexU,               sprite.TexV + sprite.TexH},
        {sprite.TexU + sprite.TexW, sprite.TexV + sprite.TexH}
    };

    // Calculate rotation transformations (future implementation)
    float c = cos(sprite.Rotation);
    float s = sin(sprite.Rotation);
    float2x2 rotation = {c, -s, s, c}; // Standard 2D rotation matrix

    float2 coord = vertexPos[vert];
    coord *= sprite.Scale;             // Transform local vertex size
    coord = mul(coord, rotation);      // Apply rotation around top-left anchor (Possibly change to center-based rotation in the future)

    // Translate vertex to world/screen space position
    float3 coordWithDepth = float3(coord + sprite.Position.xy, sprite.Position.z);

    Output output;
    output.Position = mul(ViewProjectionMatrix, float4(coordWithDepth, 1.0f));
    output.Texcoord = texcoord[vert];
    output.Color = sprite.Color;

    return output;
}
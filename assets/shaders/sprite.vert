cbuffer UniformBuffer : register(b0, space1) {
    float4x4 u_ProjectionMatrix;
};

struct VertexInput {
    float2 position : TEXCOORD0; // location 0
    float2 texCoord : TEXCOORD1; // location 1
    float4 color    : TEXCOORD2; // location 2
};

struct VertexOutput {
    float2 texCoord : TEXCOORD0;
    float4 color    : TEXCOORD1;
    float4 position : SV_POSITION;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    output.position = mul(u_ProjectionMatrix, float4(input.position, 0.0, 1.0));
    output.texCoord = input.texCoord;
    output.color = input.color;
    return output;
}
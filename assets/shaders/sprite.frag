struct PixelInput {
    float2 texCoord : TEXCOORD0;
    float4 color    : TEXCOORD1;
    float4 position : SV_POSITION;
};

Texture2D tex0 : register(t0, space2);
SamplerState samp0 : register(s0, space2); // Shared register index space

float4 main(PixelInput input) : SV_TARGET {
    return tex0.Sample(samp0, input.texCoord) * input.color;
}
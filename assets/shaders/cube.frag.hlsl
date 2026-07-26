// Register spaces are significant for resource binding in DirectX 12, ensuring no conflicts with other resources.
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader
struct FragmentInput {
    float4 position : SV_POSITION;
    float3 normal   : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float2 uv       : TEXCOORD2;
};
struct LightData {
    float4 direction;  // directional light direction (normalized)
    float4 color;      // RGB + intensity in w
    float4 ambient;    // ambient color
    float4 cameraPos;  // camera world position (for specular later)
};
ConstantBuffer<LightData> lights : register(b0, space3);

Texture2D<float4> baseTexture : register(t0, space2);
SamplerState baseSampler : register(s0, space2);
float4 main(FragmentInput input) : SV_TARGET {
    float3 normal = normalize(input.normal);
    float4 texColor = baseTexture.Sample(baseSampler, input.uv);
    float3 baseColor = texColor.rgb;
    
    // Lighting
    float3 lightDir = normalize(lights.direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Blinn-Phong specular
    float3 V = normalize(lights.cameraPos.xyz - input.worldPos); // View Dir: Surf -> Camera
    float3 H = normalize(lightDir + V); // Halfway vector
    float spec = pow(max(dot(normal, H), 0.0), 2.0);  // 32 = shininess

    float3 finalColor = baseColor * (lights.ambient.xyz + lights.color.xyz * diff) 
                    + lights.color.xyz * spec * 0.5;
    return float4(finalColor, 1.0);
}
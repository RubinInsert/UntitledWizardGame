struct FragmentInput {
    float4 position : SV_POSITION;
    float3 normal   : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};
struct LightData {
    float4 direction;  // directional light direction (normalized)
    float4 color;      // RGB + intensity in w
    float4 ambient;    // ambient color
    float4 cameraPos;  // camera world position (for specular later)
};
ConstantBuffer<LightData> lights : register(b0, space3);
float4 main(FragmentInput input) : SV_TARGET {
    float3 normal = normalize(input.normal);
    float3 baseColor = abs(normal); // Different color per face axis
    
    // Lighting
    float3 lightDir = normalize(lights.direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    
    float3 finalColor = baseColor * (lights.ambient.xyz + lights.color.xyz * diff);
    return float4(finalColor, 1.0);
}
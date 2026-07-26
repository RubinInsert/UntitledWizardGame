struct FragmentInput {
    float4 position : SV_POSITION;
    float3 normal   : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

float4 main(FragmentInput input) : SV_TARGET {
    float3 N = normalize(input.normal);
    float3 baseColor = abs(N); // Different color per face axis
    
    // Lighting
    float3 lightDir = normalize(float3(0.5, 1.0, 0.3));
    float diff = max(dot(N, lightDir), 0.0);
    float3 ambient = float3(0.1, 0.1, 0.15);
    
    float3 finalColor = baseColor * (ambient + diff * 0.9);
    return float4(finalColor, 1.0);
}
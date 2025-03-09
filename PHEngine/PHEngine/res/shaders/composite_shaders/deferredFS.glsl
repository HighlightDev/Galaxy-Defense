#version 440

layout(location = 0) out vec3 gBuffer_Position;
layout(location = 1) out vec3 gBuffer_Normal;
layout(location = 2) out vec3 gBuffer_Albedo;
layout(location = 3) out vec2 gBuffer_MetallicRoughness;
layout(location = 4) out vec4 gBuffer_Emission;

in MATERIAL_VS_OUTPUT VsOutput;

void main()
{
    vec3 albedoColor = GetMaterialAlbedo(VsOutput);
    vec2 metallicRoughnessFactor = GetMaterialMetallicRoughness(VsOutput);
    vec3 worldNormal = GetMaterialWorldNormal(VsOutput);
    vec4 emissionColor = GetMaterialEmission(VsOutput);

    gBuffer_Position = VsOutput.WorldCoordinates.xyz;
    gBuffer_Normal = worldNormal;
    gBuffer_Albedo = albedoColor;
    gBuffer_MetallicRoughness = metallicRoughnessFactor;
    gBuffer_Emission = emissionColor;
}
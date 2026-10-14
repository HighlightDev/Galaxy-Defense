#version 440 core

layout(location = 0) out vec3 gBuffer_Position;
layout(location = 1) out vec3 gBuffer_Normal;
layout(location = 2) out vec3 gBuffer_Albedo;
layout(location = 3) out vec2 gBuffer_MetallicRoughness;
layout(location = 4) out vec4 gBuffer_Emission;

in MATERIAL_VS_OUTPUT VsOutput;
flat in FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

void main()
{
    vec3 albedoColor = GetMaterialAlbedo(VsOutput, FlatVsOutput);
    vec2 metallicRoughnessFactor = GetMaterialMetallicRoughness(VsOutput, FlatVsOutput);
    vec3 worldNormal = GetMaterialWorldNormal(VsOutput, FlatVsOutput);
    vec4 emissionColor = GetMaterialEmission(VsOutput, FlatVsOutput);

    gBuffer_Position = VsOutput.WorldCoordinates.xyz;
    gBuffer_Normal = worldNormal;
    gBuffer_Albedo = albedoColor;
    gBuffer_MetallicRoughness = metallicRoughnessFactor;
    gBuffer_Emission = emissionColor;
}
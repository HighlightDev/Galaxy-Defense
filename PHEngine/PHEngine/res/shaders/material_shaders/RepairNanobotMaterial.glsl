#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 coreColor;      // hot white center (faces the camera)
uniform vec3 haloColor;      // green rim/halo
uniform vec3 cameraPosition;
uniform float opacity;

// A tiny glowing nanobot: white where the surface faces the camera, fading to a green rim — bloom turns the rim into a
// soft halo. Reads as a white core wrapped in a green glow as it rides along the beam.
vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 normal = normalize(materialIn.WorldNormal);
    vec3 viewDir = normalize(cameraPosition - materialIn.WorldCoordinates.xyz);
    float facing = clamp(dot(viewDir, normal), 0.0, 1.0);
    return mix(haloColor, coreColor, pow(facing, 2.0));
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return opacity;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

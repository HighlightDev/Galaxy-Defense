#version 440

#include "materialCommon.incl.glsl"

uniform float opacity;
uniform float clipRadius;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
    vec2 texCoords = vec2(0.5) - materialIn.TextureCoordinates.xy;
    float distance = length(texCoords);
    return opacity * (1.0 - smoothstep(0.0, clipRadius, distance));
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
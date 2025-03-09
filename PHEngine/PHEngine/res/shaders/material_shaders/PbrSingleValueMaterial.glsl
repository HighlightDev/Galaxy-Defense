#version 440

#include "materialCommon.incl.glsl"

uniform vec3 albedo;
uniform float metallicValue;
uniform float roughnessValue;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    return albedo;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec2(metallicValue, roughnessValue);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
    return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return normalize(materialIn.WorldNormal);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
#version 440 core

#include "materialCommon.incl.glsl"

uniform float opacity;
uniform vec3 albedoColor;
uniform vec3 blendColor;
uniform float blendFactor;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    return mix(albedoColor, blendColor, blendFactor);
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
    return opacity;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}

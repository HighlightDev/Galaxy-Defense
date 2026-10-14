#version 440 core

#include "materialCommon.incl.glsl"

uniform samplerCube spaceTexture;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return texture(spaceTexture, materialIn.TextureCoordinates.xyz).rgb;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec2(0.0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0.0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}
#version 440 core

#include "materialCommon.incl.glsl"

uniform sampler2D reflectionTexture;
uniform sampler2D ground;
uniform sampler2D dudv;

uniform float distortion;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 ndc = materialIn.ProjectedCoordinates.xyz / materialIn.ProjectedCoordinates.w;
    vec2 texSpaceCoords = (ndc.xy * 0.5) + 0.5;
    vec2 texCoords = materialIn.TextureCoordinates.xy * 10;

    vec2 distortionVec = (texture(dudv, texCoords + distortion).rg * 2.0) - 1.0;

    vec3 groundColor = texture(ground, texCoords).rgb;
    float groundMixPct = pow(groundColor.r, 4);

    return mix(texture(reflectionTexture, clamp(texSpaceCoords + distortionVec * 0.01, 0.0, 1.0)).rgb, groundColor, groundMixPct);
}

float GetMaterialRoughness(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 0.0;
}

float GetMaterialMetallic(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 0.0;
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
    return vec3(0, 0, 1);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}
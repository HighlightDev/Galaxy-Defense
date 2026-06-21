#version 440 core

#include "materialCommon.incl.glsl"

uniform sampler2D albedo;
uniform sampler2D dudv;

uniform float distortion;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec2 texCoords = materialIn.TextureCoordinates.xy;
    vec2 distortionVec = (texture(dudv, texCoords + distortion * 0.05).rg * 2.0) - 1.0;
    const float brightnessCorrectionFactor = 1.0 / 2.2;

    vec3 texelColor = texture(albedo, clamp(materialIn.TextureCoordinates.xy + distortionVec * 0.05, 0.0, 1.0)).rgb;
    texelColor = pow(texelColor, vec3(brightnessCorrectionFactor));
    return texelColor;
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
    return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 color;
uniform float radius;
uniform vec3 world_spacestation_position;

float blendFactor;

const float c_markerSqrWidth = 100.0;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 pixelAndCenterVec = world_spacestation_position - materialIn.WorldCoordinates.xyz;
    float sqrPixelDistanceFromCenter = dot(pixelAndCenterVec, pixelAndCenterVec);
    float sqrRadius = radius * radius;
    float innerRadiusSqrDistance = sqrRadius - c_markerSqrWidth;
    blendFactor = 0.6 * smoothstep(innerRadiusSqrDistance, sqrRadius, sqrPixelDistanceFromCenter)
        * (1.0 - step(sqrRadius, sqrPixelDistanceFromCenter));
    return color;
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
    return blendFactor;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

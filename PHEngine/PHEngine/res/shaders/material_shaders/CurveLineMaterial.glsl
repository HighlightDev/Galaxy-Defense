#version 440 core

#include "materialCommon.incl.glsl"

uniform float opacity;
uniform vec3 color;

const float c_lineWidthPct = 0.2; // % from total route
const float c_gapWidthPct = 0.15; // % from line width

float getLineOpacity(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    float xCoordinate = materialIn.TextureCoordinates.x;
    float gapWidthPct = (c_lineWidthPct * c_gapWidthPct);
    float totalLineWidthPct = c_lineWidthPct + gapWidthPct;
    float fractPart = fract(xCoordinate / totalLineWidthPct);
    return step(c_gapWidthPct, fractPart);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
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
    return getLineOpacity(materialIn, flatMaterialIn) * opacity;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

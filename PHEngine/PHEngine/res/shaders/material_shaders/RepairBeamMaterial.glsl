#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 coreColor; // hot white-green crest
uniform vec3 beamColor; // main green body
uniform vec3 glowColor; // dimmer green base
uniform float opacity;
uniform float timeSec;

const float TAU = 6.28318530718;

// TexCoord.x sweeps around the tube, TexCoord.y runs along the beam. Energy flows along the length and the brightest
// crests burn toward the hot core colour, giving the soft-glow -> main -> core read of a healing beam.
vec3 getColor(in vec2 uv)
{
    float flow = 0.5 + 0.5 * sin((uv.y * 5.0 - timeSec * 4.0) * TAU);
    float pulse = 0.55 + 0.45 * flow;
    vec3 col = mix(glowColor, beamColor, pulse);
    col = mix(col, coreColor, pow(flow, 4.0) * 0.7);
    return col;
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return getColor(materialIn.TextureCoordinates.xy);
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

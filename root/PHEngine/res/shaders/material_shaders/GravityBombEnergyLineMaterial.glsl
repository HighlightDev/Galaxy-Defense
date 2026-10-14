#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 color; // dim base color of the strip
uniform vec3 glowColor; // bright energy running along it
uniform float opacity;
uniform float timeSec;

const float TAU = 6.28318530718;

// Travelling highlights running along the length (x) of the strip, layered for an energetic flow.
float energyFlow(in vec2 uv)
{
    float wave1 = sin((uv.x * 5.0 - timeSec * 2.5) * TAU);
    float wave2 = sin((uv.x * 9.0 + timeSec * 1.7) * TAU);
    float flow = 0.5 + 0.5 * (wave1 * 0.6 + wave2 * 0.4);
    return pow(flow, 4.0);
}

// Intensity of the bright energy, concentrated along the core of the strip and pulsing as it flows.
float computeEnergyIntensity(in vec2 uv)
{
    float edge = abs(uv.y - 0.5) * 2.0; // 0 at centerline, 1 at the edges
    float core = 1.0 - edge;
    float flow = energyFlow(uv);
    return clamp(flow * pow(core, 1.5), 0.0, 1.0);
}

// Slowly warping shimmer that reads as space/matter being distorted around the strip.
vec3 computeBaseColor(in vec2 uv)
{
    float edge = abs(uv.y - 0.5) * 2.0;
    float shimmer = 0.5 + 0.5 * sin((uv.x * 3.0 + timeSec * 1.1) * TAU + edge * 4.0);
    return color * (0.35 + 0.4 * shimmer);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return computeBaseColor(materialIn.TextureCoordinates.xy);
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
    // No dashes; instead feather the alpha toward the two long edges so the strip has soft, glowing edges.
    float edge = abs(materialIn.TextureCoordinates.y - 0.5) * 2.0;
    return opacity * (1.0 - pow(edge, 2.0));
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    // Emit the bright running energy (alpha = intensity) so the forward-no-lit pass blends it over the base color and
    // the bloom pass picks up the highlights.
    return vec4(glowColor, computeEnergyIntensity(materialIn.TextureCoordinates.xy));
}

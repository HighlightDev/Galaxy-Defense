#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 color;     // deep base color (crimson)
uniform vec3 glowColor; // hot mid-tone energy (pink)
uniform float opacity;
uniform float timeSec;

const float TAU = 6.28318530718;
const vec3 PLASMA_WHITE = vec3(1.0, 0.97, 0.97);

// uv.x runs 0 at the trail tail (oldest) -> 1 at the head (newest, the missile). Everything fades toward the tail.
float lengthFade(in vec2 uv)
{
    return uv.x * uv.x; // quadratic so the tail dissolves softly
}

float energyFlow(in vec2 uv)
{
    float wave1 = sin((uv.x * 5.0 - timeSec * 3.0) * TAU);
    float wave2 = sin((uv.x * 9.0 + timeSec * 2.0) * TAU);
    float flow = 0.5 + 0.5 * (wave1 * 0.6 + wave2 * 0.4);
    return pow(flow, 5.0);
}

float computeEnergyIntensity(in vec2 uv)
{
    float edge = abs(uv.y - 0.5) * 2.0; // 0 at centerline, 1 at the edges
    float core = 1.0 - edge;
    return clamp(energyFlow(uv) * pow(core, 1.5), 0.0, 1.0) * lengthFade(uv);
}

vec3 plasmaGradient(in float intensity)
{
    vec3 grad = mix(color, glowColor, smoothstep(0.0, 0.55, intensity));
    grad = mix(grad, PLASMA_WHITE, smoothstep(0.65, 1.0, intensity));
    return grad;
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec2 uv = materialIn.TextureCoordinates.xy;
    float edge = abs(uv.y - 0.5) * 2.0;
    float shimmer = 0.5 + 0.5 * sin((uv.x * 3.0 + timeSec * 1.4) * TAU + edge * 4.0);
    return color * (0.35 + 0.45 * shimmer);
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
    vec2 uv = materialIn.TextureCoordinates.xy;
    // Soft edges across the width AND a fade toward the tail (uv.x -> 0).
    float edge = abs(uv.y - 0.5) * 2.0;
    return opacity * (1.0 - pow(edge, 2.0)) * lengthFade(uv);
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    float intensity = computeEnergyIntensity(materialIn.TextureCoordinates.xy);
    return vec4(plasmaGradient(intensity), intensity);
}

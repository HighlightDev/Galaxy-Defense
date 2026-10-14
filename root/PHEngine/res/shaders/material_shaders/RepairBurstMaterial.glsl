#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 ringColor; // green explosion rings
uniform float opacity;
uniform float timeSec;

// Pulse timing (seconds). The burst loops on the scene clock so it stays roughly in step with the game-thread-driven
// pulse ball that flies in over burstStart seconds, then this fires the rings.
uniform float cyclePeriod; // full loop length (flight + burst + rest)
uniform float burstStart; // when the rings start within a cycle (== ball flight duration)
uniform float ringDelay; // per-ring stagger (i * ringDelay)
uniform float ringLife; // how long each ring expands + fades

// Three concentric rings expanding outward from the centre and fading, each delayed by i * ringDelay.
vec4 computeBurst(in vec2 uv)
{
    vec2 p = (uv - 0.5) * 2.0; // -1..1
    float r = length(p);

    float burstTime = mod(timeSec, cyclePeriod) - burstStart;

    vec3 color = vec3(0.0);
    float alpha = 0.0;

    for (int i = 0; i < 3; ++i) {
        float ringTime = burstTime - float(i) * ringDelay;
        if (ringTime < 0.0 || ringTime > ringLife)
            continue;

        float k = ringTime / ringLife; // 0 -> 1 over the ring's life
        float radius = mix(0.12, 0.95, k); // expands outward
        float fade = 1.0 - k; // fades out
        float ring = smoothstep(0.07, 0.0, abs(r - radius)) * fade;

        color = mix(color, ringColor, ring);
        alpha = max(alpha, ring);
    }

    return vec4(color, alpha * opacity);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return computeBurst(materialIn.TextureCoordinates.xy).rgb;
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
    return computeBurst(materialIn.TextureCoordinates.xy).a;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

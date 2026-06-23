#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 wallColorLow;     // wall tint at low HP (dark/grey)
uniform vec3 wallColorHigh;    // wall tint at full HP (lighter)
uniform vec3 fillColor;        // green heal fill rising from the bottom
uniform vec3 outlineColorLow;  // border at low HP (grey)
uniform vec3 outlineColorHigh; // border at full HP (green)
uniform float opacity;
uniform float timeSec;
uniform float healPeriod; // demo: seconds for HP to sweep 18% -> 100% before looping

// Standalone demo: HP sweeps 0.18 -> 1.0 then resets. Wire this to a real barrier's HP fraction later.
float currentHp()
{
    return mix(0.18, 1.0, fract(timeSec / healPeriod));
}

// Cheap procedural web of cracks (thin dark veins).
float crackPattern(in vec2 uv)
{
    float v = sin(uv.x * 22.0) * sin(uv.y * 18.0 + uv.x * 9.0) + 0.5 * sin(uv.y * 31.0 - uv.x * 13.0);
    return smoothstep(0.85, 1.0, abs(v));
}

// uv: x horizontal, y vertical (0 bottom -> 1 top).
vec4 computeBarrier(in vec2 uv)
{
    float hp = currentHp();

    // Wall body lightens as it heals.
    vec3 color = mix(wallColorLow, wallColorHigh, hp);

    // Green heal fill rises from the bottom: filled where uv.y < hp.
    float fillMask = smoothstep(hp + 0.015, hp - 0.015, uv.y);
    color = mix(color, fillColor, fillMask * 0.55);

    // Cracks fade out as HP climbs: gone at hp >= 0.45 (alpha = 1 - hp/0.45).
    float crackAlpha = clamp(1.0 - hp / 0.45, 0.0, 1.0);
    float cracks = crackPattern(uv) * crackAlpha;
    color = mix(color, vec3(0.02, 0.03, 0.03), cracks);

    // Border: grey -> green with HP.
    vec2 edgeDist = min(uv, vec2(1.0) - uv);
    float border = smoothstep(0.04, 0.0, min(edgeDist.x, edgeDist.y));
    color = mix(color, mix(outlineColorLow, outlineColorHigh, hp), border);

    // Semi-transparent shield body, crisp border.
    float alpha = max(0.4, border * 0.9);
    return vec4(color, alpha * opacity);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return computeBarrier(materialIn.TextureCoordinates.xy).rgb;
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
    return computeBarrier(materialIn.TextureCoordinates.xy).a;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

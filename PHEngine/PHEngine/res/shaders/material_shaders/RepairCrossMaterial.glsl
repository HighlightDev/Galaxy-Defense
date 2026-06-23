#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 coreColor; // white cross/dot
uniform vec3 armColor;  // green cross arms
uniform vec3 glowColor; // green halo rings
uniform float opacity;
uniform float timeSec;

const float TAU = 6.28318530718;

// Soft rounded bar centred on the origin: |perp| within halfWidth, |along| within halfLength, with soft edges.
float bar(in float along, in float perp, in float halfLength, in float halfWidth)
{
    float w = 1.0 - smoothstep(halfWidth - 0.02, halfWidth, abs(perp));
    float l = 1.0 - smoothstep(halfLength - 0.04, halfLength, abs(along));
    return w * l;
}

// Builds the full healing cross in the billboard's uv (0..1): three pulsing concentric halo rings, green cross arms,
// white cross arms on top, and a white centre dot. Returns rgb colour + coverage alpha.
vec4 computeCross(in vec2 uv)
{
    vec2 p = (uv - 0.5) * 2.0; // -1..1, centre at 0
    float r = length(p);

    vec3 color = vec3(0.0);
    float alpha = 0.0;

    // Three concentric halo rings, each pulsing on its own phase.
    for (int i = 0; i < 3; ++i)
    {
        float radius = 0.35 + float(i) * 0.28;
        float pulse = 0.5 + 0.5 * sin(timeSec * 4.0 - float(i) * 0.85);
        float ring = smoothstep(0.045, 0.0, abs(r - radius)) * (0.2 + 0.55 * pulse);
        color = mix(color, glowColor, ring);
        alpha = max(alpha, ring);
    }

    // Green cross arms.
    float greenArm = max(bar(p.y, p.x, 0.62, 0.15), bar(p.x, p.y, 0.62, 0.15));
    color = mix(color, armColor, greenArm);
    alpha = max(alpha, greenArm);

    // White cross arms on top (thinner, slightly shorter).
    float whiteArm = max(bar(p.y, p.x, 0.5, 0.07), bar(p.x, p.y, 0.5, 0.07));
    color = mix(color, coreColor, whiteArm);
    alpha = max(alpha, whiteArm);

    // White centre dot (radial gradient).
    float dot = smoothstep(0.2, 0.0, r);
    color = mix(color, coreColor, dot);
    alpha = max(alpha, dot);

    return vec4(color, alpha * opacity);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return computeCross(materialIn.TextureCoordinates.xy).rgb;
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
    return computeCross(materialIn.TextureCoordinates.xy).a;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

#version 440 core

#include "materialCommon.incl.glsl"

uniform vec3 amberColor;  // dark amber (bottom of the top->bottom gradient)
uniform vec3 brightColor; // bright/white (top of the gradient + highlights)
uniform float opacity;
uniform float timeSec;

const float TAU = 6.28318530718;

// Heraldic-shield half-width at height y (y in [-1,1]: +1 top apex, -1 bottom tip): a pointed apex, wide shoulders,
// straight sides, then a tapering point at the bottom.
float shieldHalfWidth(in float y)
{
    float w = 0.82;
    if (y > 0.55) {
        float k = (y - 0.55) / 0.45; // 0 at the shoulders -> 1 at the apex
        w *= sqrt(max(0.0, 1.0 - k * k));
    } else if (y < -0.15) {
        float k = (y + 0.15) / (-0.85); // 0 at the sides -> 1 at the bottom tip
        w *= (1.0 - k);
    }
    return w;
}

// Signed field to the shield outline at a given scale (>0 inside, ~0 on the edge).
float shieldField(in vec2 p, in float scale)
{
    vec2 q = p / scale;
    return shieldHalfWidth(q.y) - abs(q.x);
}

vec4 computeShield(in vec2 uv)
{
    vec2 p = (uv - 0.5) * 2.0; // -1..1, +y up
    float field = shieldField(p, 1.0);
    float inside = smoothstep(-0.02, 0.02, field);
    if (inside <= 0.0) {
        return vec4(0.0);
    }

    // Top -> bottom gradient: bright at the apex, dark amber at the tip.
    float vGrad = clamp((p.y + 1.0) * 0.5, 0.0, 1.0);
    vec3 gradColor = mix(amberColor, brightColor, vGrad);

    // Layer 2: translucent fill (the barrier shows through).
    vec3 color = gradColor;
    float alpha = 0.18;

    // Layer 1: bright outer contour following the gradient.
    float outerRim = smoothstep(0.06, 0.0, abs(field));
    color = mix(color, gradColor * 1.4 + brightColor * 0.3, outerRim);
    alpha = max(alpha, outerRim);

    // Layer 3: inner contour at 72% size, shimmering independently.
    float innerField = shieldField(p, 0.72);
    float innerShimmer = 0.5 + 0.5 * sin(timeSec * 5.0);
    float innerRim = smoothstep(0.05, 0.0, abs(innerField)) * (0.4 + 0.6 * innerShimmer);
    color = mix(color, brightColor, innerRim);
    alpha = max(alpha, innerRim);

    // Three horizontal running-wave bands, clipped to the shield shape.
    for (int i = 0; i < 3; ++i) {
        float bandY = -0.4 + float(i) * 0.4;
        float bandWave = 0.5 + 0.5 * sin(timeSec * 4.0 + float(i) * 1.2);
        float band = smoothstep(0.12, 0.0, abs(p.y - bandY)) * bandWave;
        color = mix(color, brightColor, band * 0.7);
        alpha = max(alpha, band * 0.6);
    }

    return vec4(color, alpha * inside * opacity);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return computeShield(materialIn.TextureCoordinates.xy).rgb;
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
    return computeShield(materialIn.TextureCoordinates.xy).a;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

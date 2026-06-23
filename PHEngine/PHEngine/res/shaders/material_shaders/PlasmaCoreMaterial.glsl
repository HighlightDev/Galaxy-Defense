#version 440 core

#include "materialCommon.incl.glsl"

uniform sampler2D albedo;
uniform sampler2D dudv;

uniform float distortion;
uniform vec3 tintColor; // plasma tint of the swirling core (crimson)
uniform vec3 glowColor; // bloom/emission tone (hot pink)
uniform float opacity; // overall transparency of the plasma ball

const vec3 PLASMA_WHITE = vec3(1.0, 0.97, 0.97);
const vec3 luminance = vec3(0.2126, 0.7152, 0.0722);

// Distorted sample of the swirling texture, used both for the tinted base and to drive the emissive glow.
float sampleSwirl(in vec2 texCoords)
{
    vec2 distortionVec = (texture(dudv, texCoords + distortion * 0.05).rg * 2.0) - 1.0;
    vec3 texelColor = texture(albedo, clamp(texCoords + distortionVec * 0.05, 0.0, 1.0)).rgb;
    return dot(texelColor, luminance);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    float swirl = sampleSwirl(materialIn.TextureCoordinates.xy);
    // Crimson base whose hottest swirls burn toward white, so the core reads as molten plasma rather than a flat sphere.
    vec3 base = tintColor * (0.4 + 1.6 * swirl);
    return mix(base, PLASMA_WHITE, smoothstep(0.7, 1.0, swirl));
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
    // Alpha is driven solely by the opacity uniform (c_sphereOpacity), so the ball is as solid as configured rather
    // than thinned out by the swirl pattern.
    return opacity;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    // The brighter swirls glow in the pink emission tone so the ball blooms and pulses; alpha = glow intensity so the
    // forward-no-lit pass mixes glow over the tinted base instead of replacing it entirely.
    float swirl = sampleSwirl(materialIn.TextureCoordinates.xy);
    float glow = pow(swirl, 1.5);
    return vec4(glowColor, glow);
}

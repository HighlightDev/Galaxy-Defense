#version 440

#include "materialCommon.incl.glsl"

uniform sampler2D noise;
uniform float timeSec;

vec3 getColor(in MATERIAL_VS_OUTPUT materialIn)
{
    vec2 uv = materialIn.TextureCoordinates.xy;
    uv = uv * 2. - 1.;
    uv.y += 0.25;

    vec3 noise = texture(noise, materialIn.TextureCoordinates.xy + vec2(timeSec * 0.5f, timeSec * 0.5f)).rgb;
    float intensity = (noise.r + noise.g + noise.b) / 3.0;
    float t = clamp((uv.x * -uv.x * 0.08) + 0.58, 0., 1.);
    float y = abs(intensity * -t + uv.y);
    float g = pow(y, 0.2);
    vec3 color = vec3(1.0, 1.0, 1.0) * -g + vec3(1.0, 1.0, 1.0);
    return color;
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    return getColor(materialIn);
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
    return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
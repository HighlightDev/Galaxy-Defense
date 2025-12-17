#version 440

#include "materialCommon.incl.glsl"

uniform sampler2D noise;
uniform vec3 beamMainColor;
uniform vec3 beamGlowColor;
uniform float timeSec;

vec2 hash(vec2 p)
{
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float generateNoise(in vec2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    vec2 i = floor(p + (p.x + p.y) * K1);

    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;

    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

    vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));

    return dot(n, vec3(70.0));
}

vec3 getColor(in MATERIAL_VS_OUTPUT materialIn)
{
    vec2 uv = materialIn.TextureCoordinates.xy;

    // Animate UV coordinates to create flowing energy effect
    vec2 flowUV1 = uv + vec2(timeSec * 0.3, timeSec * 0.2);
    vec2 flowUV2 = uv + vec2(-timeSec * 0.2, timeSec * 0.4);

    // Sample noise at different scales and speeds for layered electricity effect
    float noise1 = texture(noise, flowUV1 * 2.0).r;
    float noise2 = texture(noise, flowUV2 * 3.0).g;
    float noise3 = texture(noise, uv * 1.5 + timeSec * 0.15).b;

    // Combine noise layers to create electrical arcing pattern
    float energyPattern = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;

    // Add pulsing intensity
    float pulse = sin(timeSec * 2.0) * 0.3 + 0.7;
    // energyPattern *= pulse;

    // Create bright core with dimmer edges based on V coordinate
    float coreGlow = 1.0 - abs(uv.y - 0.5) * 2.0;
    coreGlow = pow(coreGlow, 1.5);

    float blendFactor = generateNoise(flowUV1 * 5.0 + vec2(timeSec * 0.5, timeSec * 0.5));

    vec3 mainColor = beamMainColor * energyPattern * (1.0 + coreGlow * 0.5);

    // Combine everything: beam color modulated by energy pattern and core glow
    vec3 electricColor = beamGlowColor * energyPattern * (1.0 + coreGlow * 2.0);
    electricColor = mix(mainColor, electricColor, blendFactor);

    // Add bright electric sparks - reduced intensity and frequency
    float spark = step(0.98, noise1) * step(0.97, noise2);
    electricColor += beamGlowColor * 0.5 * spark * pulse;

    return electricColor;
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
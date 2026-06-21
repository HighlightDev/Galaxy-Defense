#version 440 core

#include "materialCommon.incl.glsl"

// Max number of instances that can be rendered in a single draw call with one instance buffer
const int MaxInstancesPerInstanceBatch = 200;

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;

uniform sampler2D albedo_ice;
uniform sampler2D normalMap_ice;
uniform sampler2D metallicMap_ice;
uniform sampler2D roughnessMap_ice;

uniform float uvScale;
uniform vec3 cameraPosition;

uniform float freezingEffect[MaxInstancesPerInstanceBatch];
uniform float damageEffect[MaxInstancesPerInstanceBatch];

// InstanceID is carried as a flat int in FLAT_MATERIAL_VS_OUTPUT (no perspective interpolation), so every fragment of
// an instance reads exactly its own gl_InstanceID and the uniform-array slot can never bleed onto a neighbour ship.
float GetFreezingEffectForCurrentInstance(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return clamp(freezingEffect[flatMaterialIn.InstanceID], 0.0, 1.0);
}

float GetDamageEffectForCurrentInstance(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return clamp(damageEffect[flatMaterialIn.InstanceID], 0.0, 1.0);
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn);

float calculateFresnel(in vec3 viewDir, in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    float fresnel = clamp(0.0, 1.0, 1.0 - dot(viewDir, GetMaterialWorldNormal(materialIn, flatMaterialIn)));
    return pow(fresnel, 1.5);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 albedoColor = texture(albedo, materialIn.TextureCoordinates.xy * uvScale).rgb;
    vec3 albedo_ice_color = texture(albedo_ice, materialIn.TextureCoordinates.xy).rgb;
    vec3 mixedIceColor
        = mix(albedoColor, albedo_ice_color, smoothstep(0.0, 0.5, GetFreezingEffectForCurrentInstance(materialIn, flatMaterialIn)));
    return mixedIceColor;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    float metallic = texture(metallicMap, materialIn.TextureCoordinates.xy * uvScale).r;
    float roughnes = texture(roughnessMap, materialIn.TextureCoordinates.xy * uvScale).r;

    float metallic_ice = texture(metallicMap_ice, materialIn.TextureCoordinates.xy).r;
    float roughnes_ice = texture(roughnessMap_ice, materialIn.TextureCoordinates.xy).r;

    vec2 mixedIceMetallicRoughness = mix(
        vec2(metallic, roughnes),
        vec2(metallic_ice, roughnes_ice),
        smoothstep(0.0, 0.5, GetFreezingEffectForCurrentInstance(materialIn, flatMaterialIn)));

    return mixedIceMetallicRoughness;
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return texture(ambientOcclusionMap, materialIn.TextureCoordinates.xy * uvScale).r;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 1.0;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 normal = (texture(normalMap, materialIn.TextureCoordinates.xy * uvScale).rgb * 2.0) - 1.0;
    vec3 normal_ice = (texture(normalMap, materialIn.TextureCoordinates.xy * uvScale).rgb * 2.0) - 1.0;
    vec3 mixedIceNormalTangentSpace
        = mix(normal, normal_ice, smoothstep(0.0, 0.5, GetFreezingEffectForCurrentInstance(materialIn, flatMaterialIn)));
    return transformNormalFromTangentSpaceToWorld(materialIn, mixedIceNormalTangentSpace);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec3 viewDir = normalize(cameraPosition - materialIn.WorldCoordinates.xyz);
    float fresnel = calculateFresnel(viewDir, materialIn, flatMaterialIn);
    float emissionPrct = smoothstep(0.0, 0.5, GetDamageEffectForCurrentInstance(materialIn, flatMaterialIn));
    vec3 outlineColor = vec3(1.0, 0.0, 0.0);
    return vec4(outlineColor, fresnel * emissionPrct);
}
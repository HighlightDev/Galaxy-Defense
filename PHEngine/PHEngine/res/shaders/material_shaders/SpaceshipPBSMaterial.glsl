#version 400

#include "materialCommon.incl.glsl"

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;
uniform float uvScale;
uniform float damageEffect;

uniform sampler2D albedo_ice;
uniform sampler2D normalMap_ice;
uniform sampler2D metallicMap_ice;
uniform sampler2D roughnessMap_ice;
uniform float freezingEffect;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec3 albedoColor = texture(albedo, materialIn.TextureCoordinates.xy * uvScale).rgb;
    vec3 albedo_ice_color = texture(albedo_ice, materialIn.TextureCoordinates.xy).rgb;
    vec3 damageColor = vec3(1.0, 0.0, 0.0);
    vec3 mixedDamageColor = mix(albedoColor, damageColor, smoothstep(0.0, 0.5, damageEffect));
    vec3 mixedIceColor = mix(mixedDamageColor, albedo_ice_color, smoothstep(0.0, 0.5, freezingEffect));
    return mixedIceColor;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
	float metallic = texture(metallicMap, materialIn.TextureCoordinates.xy * uvScale).r;
	float roughnes = texture(roughnessMap, materialIn.TextureCoordinates.xy * uvScale).r;

    float metallic_ice = texture(metallicMap_ice, materialIn.TextureCoordinates.xy).r;
	float roughnes_ice = texture(roughnessMap_ice, materialIn.TextureCoordinates.xy).r;

    vec2 mixedIceMetallicRoughness = mix(vec2(metallic, roughnes), vec2(metallic_ice, roughnes_ice), smoothstep(0.0, 0.5, freezingEffect));

    return (1.0 - step(0.01, damageEffect)) * mixedIceMetallicRoughness;
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(ambientOcclusionMap, materialIn.TextureCoordinates.xy * uvScale).r;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{   
    vec3 normal = (texture(normalMap, materialIn.TextureCoordinates.xy * uvScale).rgb * 2.0) - 1.0;
    vec3 normal_ice = (texture(normalMap, materialIn.TextureCoordinates.xy * uvScale).rgb * 2.0) - 1.0;
    vec3 mixedIceNormalTangentSpace = mix(normal, normal_ice, smoothstep(0.0, 0.5, freezingEffect));
    return transformNormalFromTangentSpaceToWorld(materialIn,mixedIceNormalTangentSpace);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
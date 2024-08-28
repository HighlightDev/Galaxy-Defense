#version 400

#include "materialCommon.incl.glsl"

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;
uniform float uvScale;
uniform vec3 cameraPosition;
uniform vec3 outlineColor;

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn);

float CalculateFresnel(in vec3 viewDir, in MATERIAL_VS_OUTPUT materialIn)
{
	float fresnel = clamp(0.0, 1.0, 1.0 - dot(viewDir, GetMaterialWorldNormal(materialIn)));
	return pow(fresnel, 2.0);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(albedo, materialIn.TextureCoordinates.xy * uvScale).rgb;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
	float metallic = texture(metallicMap, materialIn.TextureCoordinates.xy * uvScale).r;
	float roughnes = texture(roughnessMap, materialIn.TextureCoordinates.xy * uvScale).r;

	return vec2(metallic, roughnes);
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
	vec3 tangentSpaceNormal = (texture(normalMap, materialIn.TextureCoordinates.xy * uvScale).rgb * 2.0 - 1.0);
	return transformNormalFromTangentSpaceToWorld(materialIn, tangentSpaceNormal);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
	vec3 viewDir = normalize(cameraPosition - materialIn.WorldCoordinates.xyz);
	float fresnel = CalculateFresnel(viewDir, materialIn);
	return vec4(outlineColor, fresnel);
}

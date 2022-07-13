#version 400

#include "materialCommon.incl"

uniform sampler2D albedo;
uniform float uvScale;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	vec3 texelColor = texture(albedo, materialIn.TextureCoordinates.xy * uvScale).rgb;
	return texelColor * 4.5;
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

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0);
}
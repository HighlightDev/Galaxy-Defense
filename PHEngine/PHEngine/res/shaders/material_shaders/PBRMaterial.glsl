#version 400

#include "materialCommon.incl"

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(albedo, materialIn.TextureCoordinates.xy).rgb;
}

float GetMaterialRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(roughnessMap, materialIn.TextureCoordinates.xy).r;
}

float GetMaterialMetallic(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(metallicMap, materialIn.TextureCoordinates.xy).r;
}

float GetMaterialAO(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(ambientOcclusionMap, materialIn.TextureCoordinates.xy).r;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return 1.0;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return (texture(normalMap, materialIn.TextureCoordinates.xy).rgb * 2.0 - 1.0);
}
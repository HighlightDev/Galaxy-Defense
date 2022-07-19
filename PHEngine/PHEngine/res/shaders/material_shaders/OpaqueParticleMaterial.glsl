#version 400

#include "materialCommon.incl"

uniform float opacity;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0);
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
	return opacity;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0);
}
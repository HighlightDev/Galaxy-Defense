#version 400

#include "materialCommon.incl"

uniform samplerCube dayTexture;
uniform samplerCube nightTexture;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	return texture(dayTexture, materialIn.TextureCoordinates.xyz).rgb;
}

float GetMaterialRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialMetallic(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAO(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return 1.0;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0.0);
}
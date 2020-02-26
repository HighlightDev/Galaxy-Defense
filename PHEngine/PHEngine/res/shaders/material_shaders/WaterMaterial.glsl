#version 400

#include "materialCommon.incl"

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D distortionTexture;
uniform sampler2D normalMapTexture;

uniform float moveFactor;
uniform float strengthFactor;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	vec3 startColor = vec3(1, 0 ,0);
	vec3 endColor = vec3(0, 1, 1);

	return mix(startColor, endColor, moveFactor);
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
	return vec3(0, 0 ,1);
	//(texture(normalMapTexture, materialIn.TextureCoordinates.xy).rgb * 2.0 - 1.0);
}
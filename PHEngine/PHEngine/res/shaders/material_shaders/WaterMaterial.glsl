#version 400

#include "materialCommon.incl"

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D distortionTexture;
uniform sampler2D normalMap;
uniform sampler2D distortion;
uniform sampler2D someTexture;

uniform float moveFactor;
uniform float strengthFactor;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec3 ndc = materialIn.ClippedCoordinates.xyz / materialIn.ClippedCoordinates.w;
	vec2 texSpaceCoords = (ndc.xy * 0.5) + 0.5;
	vec2 texCoords = materialIn.TextureCoordinates.xy;

	vec3 distortionTexColor = texture(distortion, texCoords).rgb;
	
	return mix(texture(reflectionTexture, texSpaceCoords).rgb, texture(refractionTexture, texCoords).rgb, pow(distortionTexColor.r, 2));
	//return texture(refractionTexture, texCoords * 0.8).rgb;
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
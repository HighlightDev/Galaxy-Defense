#version 400

#include "materialCommon.incl"

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform float distortion;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec3 ndc = materialIn.ClippedCoordinates.xyz / materialIn.ClippedCoordinates.w;
	vec2 texSpaceCoords = (ndc.xy * 0.5) + 0.5;
	vec2 texCoords = materialIn.TextureCoordinates.xy;

	vec3 reflectionTexColor = texture(reflectionTexture, texCoords).rgb;
	vec3 refractionTexColor = texture(refractionTexture, texCoords).rgb;
	
	return mix(reflectionTexColor, refractionTexColor, distortion);
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
	return vec3(0, 0, 1);
}
#version 400

#include "materialCommon.incl"

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D distortionTexture;
uniform sampler2D normalMap;
uniform sampler2D distortion;

uniform float moveFactor;
uniform float strengthFactor;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	vec2 texCoords = materialIn.TextureCoordinates.xy;

	vec2 distortedTexCoords = texture(distortion, vec2(texCoords.x + moveFactor * 0.00001, texCoords.y)).rg * 0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor * 0.00001);
	vec2 totalDistortion = (texture(distortion, distortedTexCoords).rg * 2.0 - 1.0);

	return texture(normalMap, texCoords).rgb;
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
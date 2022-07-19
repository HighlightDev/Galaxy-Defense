#version 400

#include "materialCommon.incl"

uniform sampler2D albedo;
uniform sampler2D dudv;

uniform float distortion;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	vec3 ndc = materialIn.ClippedCoordinates.xyz / materialIn.ClippedCoordinates.w;
	vec2 texSpaceCoords = (ndc.xy * 0.5) + 0.5;
	vec2 texCoords = materialIn.TextureCoordinates.xy;

	vec2 distortionVec =  (texture(dudv, texCoords + distortion * 0.5).rg * 2.0) - 1.0;

	vec3 texelColor = texture(albedo, clamp(materialIn.TextureCoordinates.xy + distortionVec, 0.0, 1.0)).rgb;
	const float brightnessCorrectionFactor = 1.0 / 2.2;
  	texelColor = pow(texelColor, vec3(brightnessCorrectionFactor));
	return texelColor;
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
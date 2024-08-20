#version 400

#include "materialCommon.incl.glsl"

uniform samplerCube dayTexture;
uniform samplerCube nightTexture;
uniform float dayTimeElapsed;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
	float nightEnable = round(step(0.5, dayTimeElapsed));
	float dayEnable = round(1.0 - nightEnable);
	
	float mixCoef = ((dayTimeElapsed * 2.0) * dayEnable) + (((1.0 - dayTimeElapsed) * 2.0) * nightEnable);

	vec3 dayColour = texture(dayTexture, materialIn.TextureCoordinates.xyz).rgb;
	vec3 nightColour = texture(nightTexture, materialIn.TextureCoordinates.xyz).rgb;

	return mix(nightColour, dayColour, mixCoef);
}

float GetMaterialRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialMetallic(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0.0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec4(0.0);
}
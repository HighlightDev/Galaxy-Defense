#version 400

#include "materialCommon.incl.glsl"

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
	vec2 texCoords = vec2(0.5) - materialIn.TextureCoordinates.xy;
	float sqrDistance = dot(texCoords, texCoords);
	return opacity * (1.0 - smoothstep(0.0, 0.25, sqrDistance));
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec3(0);
}
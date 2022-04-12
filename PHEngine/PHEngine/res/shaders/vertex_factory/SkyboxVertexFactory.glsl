#version 400

layout (location = 0) in vec3 VertexPosition;

#include "materialCommon.glsl"

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec4 GetLocalToWorldSpacePosition() 
{
	return worldMatrix * vec4(VertexPosition, 1.0);
}

vec3 GetLocalToWorldSpaceNormal()
{
	return vec3(0);
}

vec3 GetLocalToWorldSpaceTangent()
{
	return vec3(0);
}

vec3 GetLocalToWorldSpaceBitangent()
{
	return vec3(0);
}

vec2 GetLocalTexCoords()
{
	return vec2(0);
}

vec3 GetVertexColor()
{
	return vec3(0);
}

MATERIAL_VS_OUTPUT VertexFactoryGetMaterialOutput()
{
	MATERIAL_VS_OUTPUT result;

	result.TextureCoordinates = VertexPosition;

	vec4 world_pos = GetLocalToWorldSpacePosition();
	vec4 view_pos = viewMatrix * world_pos;
	vec4 clipped_pos = projectionMatrix * view_pos;

	result.WorldCoordinates = world_pos;
	result.ViewCoordinates = view_pos;
	result.ClippedCoordinates = clipped_pos;

	result.WorldNormal = GetLocalToWorldSpaceNormal();
	result.WorldTangent = GetLocalToWorldSpaceTangent();
	result.WorldBitangent = GetLocalToWorldSpaceBitangent();


	return result;
}
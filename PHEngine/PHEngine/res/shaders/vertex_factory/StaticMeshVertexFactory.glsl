#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoords;
layout (location = 3) in vec3 VertexColor;
layout (location = 4) in vec3 VertexTangent;
layout (location = 5) in vec3 VertexBitangent;

#include "materialCommon.glsl"

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

struct TRANSFORM_MATRICES
{
	mat4 WORLD;
	mat4 VIEW;
	mat4 PROJECTION;
};

struct VERTEX_FACTORY_INPUT
{
	vec3 VertexPosition;
	vec3 VertexNormal;
	vec2 VertexTexCoords;
	vec3 VertexColor;
	vec3 VertexTangent;
	vec3 VertexBitangent;
} Input;

VERTEX_FACTORY_INPUT GetVertexFactoryInput()
{
	Input.VertexPosition = VertexPosition;
	Input.VertexNormal = VertexNormal;
	Input.VertexTexCoords = VertexTexCoords;
	Input.VertexColor = VertexColor;
	Input.VertexTangent = VertexTangent;
	Input.VertexBitangent = VertexBitangent;
	return Input;
}

vec4 GetLocalToWorldSpacePosition() 
{
	return worldMatrix * vec4(VertexPosition, 1.0);
}

vec3 GetLocalToWorldSpaceNormal()
{
	return (worldMatrix * vec4(VertexNormal, 0.0)).xyz;
}

vec3 GetLocalToWorldSpaceTangent()
{
	return (worldMatrix * vec4(VertexTangent, 0.0)).xyz;
}

vec3 GetLocalToWorldSpaceBitangent()
{
	return (worldMatrix * vec4(VertexBitangent, 0.0)).xyz;
}

vec2 GetLocalTexCoords()
{
	return VertexTexCoords;
}

vec3 GetVertexColor()
{
	return VertexColor;
}

TRANSFORM_MATRICES GetTransformMatrices()
{
	TRANSFORM_MATRICES mat;
	mat.WORLD = worldMatrix;
	mat.VIEW = viewMatrix;
	mat.PROJECTION = projectionMatrix;

	return mat;
}

MATERIAL_VS_OUTPUT VertexFactoryGetMaterialOutput()
{
	MATERIAL_VS_OUTPUT result;

	result.TextureCoordinates = vec3(VertexTexCoords, 0.0);

	vec4 world_pos = GetLocalToWorldSpacePosition();
	vec4 clipped_pos = projectionMatrix * viewMatrix * world_pos;
	vec3 ndc_pos = clipped_pos.xyz / clipped_pos.w;

	result.WorldCoordinates = world_pos.xyz;
	result.ClippedCoordinates = clipped_pos;
	result.NormalizedDeviceCoordinates = ndc_pos;

	result.WorldNormal = GetLocalToWorldSpaceNormal();
	result.WorldTangent = GetLocalToWorldSpaceTangent();
	result.WorldBitangent = GetLocalToWorldSpaceBitangent();

	return result;
}
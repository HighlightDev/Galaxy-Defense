#version 400

layout (location = 0) in vec3 VertexPosition;

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
} Input;

VERTEX_FACTORY_INPUT GetVertexFactoryInput()
{
	Input.VertexPosition = VertexPosition;
	return Input;
}

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

	result.TextureCoordinates = VertexPosition;

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
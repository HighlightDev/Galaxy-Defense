#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoords;
layout (location = 3) in vec3 VertexColor;
layout (location = 4) in vec3 VertexTangent;
layout (location = 5) in vec3 VertexBitangent;
layout(location = 6) in vec3 VertexBlendWeights;
layout(location = 7) in ivec3 VertexBlendIndices;

#include "materialCommon.glsl"

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 bonesMatrices[MaxBones];

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
	vec3 VertexBlendWeights;
	ivec3 VertexBlendIndices;
} Input;

VERTEX_FACTORY_INPUT GetVertexFactoryInput()
{
	Input.VertexPosition = VertexPosition;
	Input.VertexNormal = VertexNormal;
	Input.VertexTexCoords = VertexTexCoords;
	Input.VertexColor = VertexColor;
	Input.VertexTangent = VertexTangent;
	Input.VertexBitangent = VertexBitangent;
	Input.VertexBlendWeights = VertexBlendWeights;
	Input.VertexBlendIndices = VertexBlendIndices;
	return Input;
}

vec4 ApplySkinningToVec(vec4 vector)
{
	vec4 skinnedVec = vec4(0);
	 for (int i = 0; i < MaxWeights; i++)
	{
		int blendIndex = VertexBlendIndices[i];
		if (blendIndex < 0)
			continue;

		float blendWeight = VertexBlendWeights[i];
		skinnedVec += ((bonesMatrices[blendIndex]  * vector) * blendWeight);
	}

	return skinnedVec;
}

vec4 GetLocalToWorldSpacePosition() 
{
	return worldMatrix * ApplySkinningToVec(vec4(VertexPosition, 1.0));
}

vec3 GetLocalToWorldSpaceNormal()
{
	vec4 nSkinnedNormal = normalize(ApplySkinningToVec(vec4(VertexNormal, 0.0)));
	
	return (worldMatrix * nSkinnedNormal).xyz;
}

vec3 GetLocalToWorldSpaceTangent()
{
	vec4 nSkinnedTangent = normalize(ApplySkinningToVec(vec4(VertexTangent, 0.0)));
	
	return (worldMatrix * nSkinnedTangent).xyz;
}

vec3 GetLocalToWorldSpaceBitangent()
{
	vec4 nSkinnedBitangent = normalize(ApplySkinningToVec(vec4(VertexBitangent, 0.0)));
	
	return (worldMatrix * nSkinnedBitangent).xyz;
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
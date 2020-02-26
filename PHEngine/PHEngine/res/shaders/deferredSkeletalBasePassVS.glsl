#version 400

#define MaxWeights 3
#define MaxBones 55
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;
layout (location = 6) in vec3 blendWeights;
layout (location = 7) in ivec3 blendIndices;

out VS_OUT
{
	vec3 world_normal;
	vec3 world_tangent;
	vec3 world_bitangent;
	vec3 world_position;
	vec2 tex_coords;
} vertex_shader_result;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 bonesMatrices[MaxBones];

void main(void)
{
	vertex_shader_result.world_normal = (worldMatrix * vec4(normal, 0.0)).xyz;
	vertex_shader_result.world_tangent = (worldMatrix * vec4(tangent, 0.0)).xyz;
	vertex_shader_result.world_bitangent = (worldMatrix * vec4(bitangent, 0.0)).xyz;
	vertex_shader_result.tex_coords = texCoord;

	vec4 localSpacePosition = vec4(position, 1.0);

	vec4 localSpaceSkinnedVertex = vec4(0);
	 for (int i = 0; i < MaxWeights; i++)
	{
		int blendIndex = blendIndices[i];
		if (blendIndex < 0)
			continue;

		float blendWeight = blendWeights[i];
		localSpaceSkinnedVertex += ((bonesMatrices[blendIndex]  * localSpacePosition) * blendWeight);
	}

	vec4 worldPosition = worldMatrix * localSpaceSkinnedVertex;

	vertex_shader_result.world_position = worldPosition.xyz;

	gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
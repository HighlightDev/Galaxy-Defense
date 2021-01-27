#version 400

#define MaxWeights 4
#define MaxBones 155
layout (location = 0) in vec3 vertex_position;
layout (location = 6) in vec4 blendWeights;
layout (location = 7) in ivec4 blendIndices;

uniform mat4 worldMatrix;
uniform mat4 shadowViewMatrix;
uniform mat4 shadowProjectionMatrix;
uniform mat4 bonesMatrices[MaxBones];

out vec4 worldPosition;

void main(void)
{
	vec4 position = vec4(vertex_position, 1.0);

	vec4 localSpaceSkinnedVertex = vec4(0);
	for (int i = 0; i < MaxWeights; i++)
	{
		int blendIndex = blendIndices[i];

		float blendWeight = blendWeights[i];
		localSpaceSkinnedVertex += ((bonesMatrices[blendIndex] * position) * blendWeight);
	}

	worldPosition = worldMatrix * localSpaceSkinnedVertex;
	gl_Position = shadowProjectionMatrix * shadowViewMatrix * worldPosition;
}
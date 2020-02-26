#version 400

#define MaxWeights 3
#define MaxBones 55
layout (location = 0) in vec3 vertex_position;
layout (location = 6) in vec3 blendWeights;
layout (location = 7) in ivec3 blendIndices;

uniform mat4 worldMatrix;
uniform mat4 shadowViewMatrix;
uniform mat4 shadowProjectionMatrix;
uniform mat4 bonesMatrices[MaxBones];

void main(void)
{
	vec4 worldPosition = vec4(vertex_position, 1.0);

	vec4 localSpaceSkinnedVertex = vec4(0);
	for (int i = 0; i < MaxWeights; i++)
	{
		int blendIndex = blendIndices[i];
		if (blendIndex < 0)
		{
			continue;
		}

		float blendWeight = blendWeights[i];
		localSpaceSkinnedVertex += ((bonesMatrices[blendIndex] * worldPosition) * blendWeight);
	}

	worldPosition = worldMatrix * localSpaceSkinnedVertex;
	gl_Position = shadowProjectionMatrix * shadowViewMatrix * worldPosition;
}
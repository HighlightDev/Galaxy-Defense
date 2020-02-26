#version 400

#define MaxWeights 3
#define MaxBones 55
layout (location = 0) in vec3 vertex_position;

uniform mat4 worldMatrix;

void main(void)
{
	gl_Position = worldMatrix * vec4(vertex_position, 1.0);
}
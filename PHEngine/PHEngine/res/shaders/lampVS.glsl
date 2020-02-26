#version 330

layout (location = 0) in vec3 vertex;

uniform mat4 viewMatrix;
uniform mat4 worldMatrix;

out vec2 billboardScale;

void main(void)
{
	billboardScale = vec2(worldMatrix[0][0], worldMatrix[1][1]);
	gl_Position = viewMatrix * worldMatrix * vec4(vertex, 1.0);
}
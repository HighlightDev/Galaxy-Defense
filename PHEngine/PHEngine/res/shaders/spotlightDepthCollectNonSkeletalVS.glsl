#version 400

layout (location = 0) in vec3 vertex_position;

uniform mat4 worldMatrix;
uniform mat4 shadowViewMatrix;
uniform mat4 shadowProjectionMatrix;

out vec4 worldPosition;

void main(void)
{
	worldPosition = worldMatrix * vec4(vertex_position, 1.0);
	gl_Position = shadowProjectionMatrix * shadowViewMatrix * worldPosition;
}
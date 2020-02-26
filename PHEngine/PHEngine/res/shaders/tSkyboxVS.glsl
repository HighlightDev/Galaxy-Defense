#version 400

layout (location = 0) in vec3 vertex;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 texCoords;

void main()
{
	texCoords = vertex;
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(vertex, 1.0);
}
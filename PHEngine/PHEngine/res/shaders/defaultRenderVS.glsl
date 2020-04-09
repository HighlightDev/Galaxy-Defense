#version 330

layout (location = 0) in vec3 position;

uniform mat4 worldViewProjectionMatrix;

void main()
{
    gl_Position = worldViewProjectionMatrix * vec4(position, 1.0);
}
#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec4 color;

void main(void)
{
	FragColor = color;
}
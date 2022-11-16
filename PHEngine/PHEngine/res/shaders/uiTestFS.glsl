#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec4 uiColor;

void main(void)
{
	FragColor = uiColor;
}
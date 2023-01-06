#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform float opacity;

void main(void)
{
	FragColor = vec4(color, opacity);
}
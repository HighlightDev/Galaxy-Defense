#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D image;
uniform float opacity;

void main(void)
{
	FragColor = opacity * texture(image, texCoords);
}
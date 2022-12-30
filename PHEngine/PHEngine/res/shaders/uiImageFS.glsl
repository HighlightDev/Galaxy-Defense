#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D image;
uniform float opacity;
uniform bool hili;

void main(void)
{
	FragColor = hili ? vec4(1) : (opacity * texture(image, texCoords));
}
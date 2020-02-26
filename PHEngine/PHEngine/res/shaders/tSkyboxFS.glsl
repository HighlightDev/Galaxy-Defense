#version 400

layout (location = 0) out vec4 FragColor;

uniform samplerCube dayTexture;

in vec3 texCoords;

void main(void)
{
	vec4 texDayColour = texture(dayTexture, texCoords);

	FragColor = texDayColour;
}

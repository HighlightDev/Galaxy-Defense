#version 400

layout (location = 0) out vec4 FragColor;

uniform samplerCube cubeTexture;

in vec3 texCoords;

void main(void)
{
	vec4 texColour = texture(cubeTexture, texCoords);

	FragColor = texColour;
}

#version 400

layout (location = 0) out vec4 FragColor;

in vec2 gs_out_texCoords;

uniform sampler2D billboardTexture;

void main(void) 
{
	FragColor = texture(billboardTexture, gs_out_texCoords);
}
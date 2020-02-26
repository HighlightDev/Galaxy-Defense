#version 400

layout (location = 0) out vec4 FragColor;

smooth in vec2 texCoords;

uniform sampler2D billboardTexture;

void main(void)
{
	vec4 resultPixel = texture(billboardTexture, texCoords);
	if (resultPixel.a < 0.8) 
		discard;
	else
		FragColor = resultPixel;
}	
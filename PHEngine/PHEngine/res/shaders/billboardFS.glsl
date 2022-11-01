#version 400

layout (location = 0) out vec4 FragColor;

in vec2 gs_out_texCoords;

uniform sampler2D billboardTexture;

void main(void) 
{
    vec4 color = texture(billboardTexture, gs_out_texCoords);
    //float texCoordLen = length(vec2(0.5) - gs_out_texCoords);
    //float edgeAlpha = 1.0 - smoothstep(0.3, 0.5, texCoordLen);
	//FragColor = vec4(color.rgb, edgeAlpha * 0.5);
	FragColor = color;
}
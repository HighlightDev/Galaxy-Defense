#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D fontAtlas;
uniform vec3 color;

const float softWidth = 0.1;
const float boldWidth = 0.4;

void main(void)
{
    float alpha = 1.0 - texture(fontAtlas, texCoords).a;
    float resultAlpha = 1.0 - smoothstep(boldWidth, softWidth + boldWidth, alpha);
    FragColor = vec4(color, resultAlpha);
}
#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;
in vec3 out_color;

uniform sampler2D fontAtlas;

const float softWidth = 0.1;
const float boldWidth = 0.4;

void main(void)
{
    float alpha = 1.0 - texture(fontAtlas, texCoords).a;
    float resultAlpha = 1.0 - smoothstep(boldWidth, softWidth + boldWidth, alpha);
    FragColor = vec4(out_color, resultAlpha);
}
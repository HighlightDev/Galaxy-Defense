#version 400

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D fontAtlas;
uniform vec3 color;
uniform float shadowWidth = 0.2;
uniform vec2 shadowOffset = vec2(-0.002, -0.002);
uniform float opacity = 1.0;

const float softWidth = 0.2;
const float boldWidth = 0.4;

const float shadowStartFrom = 0.4;

void main(void)
{
    float alpha = 1.0 - texture(fontAtlas, texCoords).a;
    float shadowAlpha = 1.0 - texture(fontAtlas, texCoords + shadowOffset).a;
    
    float fontSmoothEdgeAlpha = 1.0 - smoothstep(boldWidth, softWidth + boldWidth, alpha);
    float shadowSmoothEdgeAlpha = 1.0 - smoothstep(shadowStartFrom, shadowStartFrom +  shadowWidth, shadowAlpha);

    float overallAlpha = fontSmoothEdgeAlpha + (1.0 - fontSmoothEdgeAlpha) * shadowSmoothEdgeAlpha;
    vec3 resultColor = mix((color * 0.5), color, fontSmoothEdgeAlpha / overallAlpha);
    FragColor = vec4(resultColor, overallAlpha * opacity);
}
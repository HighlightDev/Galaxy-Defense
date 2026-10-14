#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec3 emptyColor;
uniform vec3 filledColor;
uniform float fillPercentValue;
uniform float opacity;
uniform float borderRadius = 0;
uniform vec2 widthAndHeight;

float udRoundBox(in vec2 pixelPos, in vec2 centerPos, float radius)
{
    return length(max(abs(pixelPos) - centerPos + vec2(radius), vec2(0.0))) - radius;
}

void main(void)
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 center = widthAndHeight * 0.5;
    float borderRadiusOpacityCoef
        = 1.0 - (step(0.01, borderRadius) * step(0.0, udRoundBox(pixelPos - center, center, borderRadius)));
    float fillValue = 1.0 - step(fillPercentValue, texCoords.x);
    FragColor = vec4(mix(emptyColor, filledColor, fillValue), borderRadiusOpacityCoef * opacity);
}
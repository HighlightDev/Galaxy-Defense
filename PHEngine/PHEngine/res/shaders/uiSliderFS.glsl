#version 440

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform float opacity;
uniform vec3 color;
uniform float borderRadius = 0;
uniform vec2 widthAndHeight;

subroutine vec4 renderSlider();
subroutine uniform renderSlider process;

float udRoundBox(in vec2 pixelPos, in vec2 centerPos, float radius)
{
    return length(max(abs(pixelPos) - centerPos + vec2(radius), vec2(0.0))) - radius;
}

subroutine(renderSlider) vec4 renderSliderLine()
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 center = widthAndHeight * 0.5;
    float borderRadiusOpacityCoef
        = 1.0 - (step(0.01, borderRadius) * step(0.0, udRoundBox(pixelPos - center, center, borderRadius)));
    return vec4(color, borderRadiusOpacityCoef * opacity);
}

subroutine(renderSlider) vec4 renderSliderBlob()
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 center = widthAndHeight * 0.5;
    float radius = min(widthAndHeight.x, widthAndHeight.y) * 0.5;
    float borderRadiusOpacityCoef = 1.0 - (step(0.01, radius) * step(0.0, udRoundBox(pixelPos - center, center, radius)));
    float smoothOpacity = smoothstep(0.0, 0.1, 1.0 - length(pixelPos - center) / radius);
    return vec4(color, borderRadiusOpacityCoef * opacity * smoothOpacity);
}

void main(void)
{
    FragColor = process();
}
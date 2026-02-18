#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform float opacity;
uniform float borderRadius = 0;
uniform vec2 widthAndHeight;

uniform bool isRoundTop;
uniform bool isRoundBottom;

float udRoundBox(in vec2 pixelPos, in vec2 centerPos, float radius)
{
    return (length(max(abs(pixelPos) - centerPos + vec2(radius), vec2(0.0))) - radius);
}

void main(void)
{
    vec2 center = widthAndHeight * 0.5;
    vec2 currentPixelPos = texCoords * widthAndHeight;

    int pixelsToCheck = 1;
    float sumNeighborRounding = 0.0;
    for (int i = -pixelsToCheck; i <= pixelsToCheck; i += 1) {
        for (int j = -pixelsToCheck; j <= pixelsToCheck; j += 1) {
            vec2 neighborPixelPos = vec2(currentPixelPos.x + float(i) * 2, currentPixelPos.y + float(j) * 2);
            vec2 direction = normalize(currentPixelPos - center);
            float roundTopCoef = step(dot(vec2(0.0, -1.0), direction), 0.0) * float(isRoundTop);
            float roundBottomCoef = step(dot(vec2(0.0, 1.0), direction), 0.0) * float(isRoundBottom);
            float coef = clamp(roundTopCoef + roundBottomCoef, 0.0, 1.0);
            sumNeighborRounding += udRoundBox(neighborPixelPos - center, center, borderRadius) * coef;
        }
    }
    int totalPixelsChecked = (pixelsToCheck * 2 + 1) * (pixelsToCheck * 2 + 1);
    float borderRadiusOpacityCoef = 1.0 - sumNeighborRounding / float(totalPixelsChecked);
    FragColor = vec4(color, borderRadiusOpacityCoef * opacity);
}
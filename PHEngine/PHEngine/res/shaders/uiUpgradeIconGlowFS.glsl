#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

// Размер квада свечения в пикселях (больше иконки на glowSizePx с каждой стороны).
uniform vec2 widthAndHeight;
// Размер исходной иконки в пикселях — задаёт радиус круга, от которого расходится свечение.
uniform vec2 iconWidthAndHeight;

uniform vec3 glowColor;
uniform float glowSizePx;
uniform float opacity;

void main(void)
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 center = widthAndHeight * 0.5;

    float iconRadius = min(iconWidthAndHeight.x, iconWidthAndHeight.y) * 0.5;
    float distPx = length(pixelPos - center);
    float aa = 1.0;

    // Свечение расходится наружу от края круга иконки.
    float glow = 1.0 - smoothstep(0.0, glowSizePx, max(distPx - iconRadius, 0.0));
    glow = pow(glow, 1.6);

    // Гасим свечение внутри круга иконки — квад свечения не должен перекрывать саму иконку.
    float outsideMask = smoothstep(iconRadius - aa, iconRadius, distPx);
    glow *= outsideMask;

    float finalA = glow * opacity;
    if (finalA < 0.01) {
        discard;
    }

    FragColor = vec4(glowColor, finalA);
}

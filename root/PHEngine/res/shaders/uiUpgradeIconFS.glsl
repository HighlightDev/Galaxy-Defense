#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D image;

uniform vec2 widthAndHeight;

uniform vec3 fillColor;
uniform vec3 borderColor;
uniform vec3 iconCustomColor;

uniform float isCustomIconColor;
uniform float fillStrength;
uniform float borderThicknessPx;
uniform float opacity;

void main(void)
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 center = widthAndHeight * 0.5;
    float halfMinDim = min(widthAndHeight.x, widthAndHeight.y) * 0.5;

    float circleRadius = max(halfMinDim - 1.0, 1.0);

    float distPx = length(pixelPos - center);
    float aa = 1.0;

    float circleMask = 1.0 - smoothstep(circleRadius - aa, circleRadius, distPx);

    float innerEdge = max(circleRadius - borderThicknessPx, 0.0);
    float borderMask = smoothstep(innerEdge - aa, innerEdge, distPx) - smoothstep(circleRadius - aa, circleRadius, distPx);
    borderMask = clamp(borderMask, 0.0, 1.0);

    float fillFalloff = 1.0 - smoothstep(0.0, circleRadius, distPx);
    vec3 fillGradient = mix(vec3(0.03, 0.05, 0.10), fillColor, fillFalloff);
    float fillAlpha = circleMask * fillStrength;

    vec2 texQuadCenter = center;
    float texHalfSize = circleRadius;
    vec2 texLocalUv = (pixelPos - texQuadCenter) / max(texHalfSize, 1.0);
    texLocalUv = texLocalUv * 0.5 + 0.5;

    vec4 texSample = vec4(0.0);
    if (texLocalUv.x >= 0.0 && texLocalUv.x <= 1.0 && texLocalUv.y >= 0.0 && texLocalUv.y <= 1.0) {
        texSample = texture(image, texLocalUv);
    }
    vec3 iconRgb = mix(texSample.rgb, iconCustomColor, isCustomIconColor);
    float iconAlpha = texSample.a * circleMask;

    vec3 baseRgb = fillGradient;
    float baseA = fillAlpha;

    vec3 afterIcon = mix(baseRgb, iconRgb, iconAlpha);
    float afterIconA = baseA + iconAlpha * (1.0 - baseA);

    vec3 finalRgb = mix(afterIcon, borderColor, borderMask);
    float finalA = afterIconA + borderMask * (1.0 - afterIconA);

    if (finalA < 0.01) {
        discard;
    }

    FragColor = vec4(finalRgb, finalA * opacity);
}

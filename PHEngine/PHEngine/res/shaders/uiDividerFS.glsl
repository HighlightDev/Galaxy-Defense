#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform float opacity;
uniform float lineWidthPx;
// Fraction of the line length over which it fades out toward each end (0 = solid, 0.5 = fade all the way).
uniform float edgeFade;
// 0 = horizontal line (across the widget), 1 = vertical line (down the widget).
uniform int orientation;
uniform vec2 widthAndHeight;
uniform vec2 screenResolution;

float project(in vec2 left, in vec2 right) {
    return dot(left, right) / max(dot(right, right), 0.001);
}

float distSquared(in vec2 vector) {
    return dot(vector, vector);
}

void main(void) {
    vec2 currentPixelPos = texCoords * widthAndHeight;
    // orientation == 1 -> vertical orientation, orientation == 2 -> horizontal orientation
    vec2 a = orientation == 1 ? vec2(widthAndHeight.x * 0.5, 0.0) : vec2(0.0, widthAndHeight.y * 0.5);
    vec2 b = orientation == 1 ? vec2(widthAndHeight.x * 0.5, widthAndHeight.y) : vec2(widthAndHeight.x, widthAndHeight.y * 0.5);

    vec2 ba = b - a;
    vec2 pixelA = currentPixelPos - a;
    float d = clamp(project(pixelA, ba), 0.0, 1.0);
    vec2 closestPointToPixel = ba * d;
    float lineMask = 1.0 - step(lineWidthPx * lineWidthPx, distSquared(pixelA - closestPointToPixel));

    // Fade toward both ends over `edgeFade` of the line length (0 = solid line). Reproduces the mockup
    // dividers' transparent -> solid -> transparent gradient.
    float edgeMask = edgeFade <= 0.0 ? 1.0 : smoothstep(0.0, edgeFade, d) * (1.0 - smoothstep(1.0 - edgeFade, 1.0, d));

    vec3 finalColor = color;
    FragColor = vec4(finalColor, opacity * lineMask * edgeMask);
}

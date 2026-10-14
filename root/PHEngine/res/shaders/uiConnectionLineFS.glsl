#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec2 widthAndHeight;

uniform vec2 startPoint;
uniform vec2 endPoint;

uniform vec3 color;

uniform float thicknessPx;
uniform float dashLengthPx;
uniform float gapLengthPx;
uniform float opacity;

void main(void)
{
    vec2 pixelPos = texCoords * widthAndHeight;
    vec2 a = startPoint * widthAndHeight;
    vec2 b = endPoint * widthAndHeight;

    vec2 pa = pixelPos - a;
    vec2 ba = b - a;
    float baLenSq = max(dot(ba, ba), 1e-6);
    float h = clamp(dot(pa, ba) / baLenSq, 0.0, 1.0);
    float distToSegment = length(pa - ba * h);

    float aa = 1.0;
    float halfThickness = thicknessPx * 0.5;
    float lineMask = 1.0 - smoothstep(halfThickness - aa, halfThickness + aa, distToSegment);

    float dashMask = 1.0;
    if (dashLengthPx > 0.0) {
        float period = dashLengthPx + max(gapLengthPx, 0.0);
        float distAlongLine = h * sqrt(baLenSq);
        float segmentPos = mod(distAlongLine, period);
        dashMask = 1.0 - smoothstep(dashLengthPx - aa, dashLengthPx + aa, segmentPos);
    }

    float alpha = lineMask * dashMask * opacity;
    FragColor = vec4(color, alpha);
}

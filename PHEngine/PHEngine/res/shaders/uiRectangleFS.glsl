#version 400

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform float opacity;
uniform float borderRadius = 0;
uniform vec2 widthAndHeight;

float udRoundBox(in vec2 pixelPos, in vec2 centerPos, float radius) {
  return length(max(abs(pixelPos) - centerPos + vec2(radius), vec2(0.0))) -
         radius;
}

void main(void) {
  vec2 pixelPos = texCoords * widthAndHeight;
  vec2 center = widthAndHeight * 0.5;
  float borderRadiusOpacityCoef =
      1.0 - (step(0.01, borderRadius) *
             step(0.0, udRoundBox(pixelPos - center, center, borderRadius)));
  FragColor = vec4(color, borderRadiusOpacityCoef * opacity);
}
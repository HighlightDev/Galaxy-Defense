#version 400

#include "materialCommon.incl.glsl"

uniform float opacity;
uniform vec3 color;
uniform float pulse_animation_time;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn) { return color; }

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn) {
  return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn) {
  return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn) {
  float animationValue = pulse_animation_time > 1.0 ? (2.0 - pulse_animation_time) : pulse_animation_time;
  float maxSqrOuterRectangleRadius = pow(0.65, 2);
  float maxSqrRectangleRadius = pow(0.6, 2);
  vec2 texelVec = materialIn.TextureCoordinates.xy - vec2(0.5);
  float sqrRectangleRadius = texelVec.x * texelVec.x + texelVec.y * texelVec.y;
  float innerRadiusOpacity = step(maxSqrRectangleRadius, sqrRectangleRadius);
  float outerRadiusOpacity = 1.0 - step(maxSqrOuterRectangleRadius, sqrRectangleRadius);

  return innerRadiusOpacity * outerRadiusOpacity * opacity * clamp(animationValue, 0.1, 1.0);
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn) {
  return vec3(0);
}

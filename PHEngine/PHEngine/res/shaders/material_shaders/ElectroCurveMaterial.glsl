#version 400

#include "materialCommon.incl"

uniform sampler2D noise;
uniform float timeSec;
uniform float opacity;

float alpha;

const float c_lineWidthPct = 0.2; // % from total route
const float c_gapWidthPct = 0.15; // % from line width

vec4 getLineColor(in MATERIAL_VS_OUTPUT materialIn) {
  float xCoordinate = materialIn.TextureCoordinates.x;
  float gapWidthPct = (c_lineWidthPct * c_gapWidthPct);
  float totalLineWidthPct = c_lineWidthPct + gapWidthPct;
  float fractPart = fract(xCoordinate / totalLineWidthPct);
  return vec4(vec3(1.0), step(c_gapWidthPct, fractPart));
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn) {
  vec4 colorResult = getLineColor(materialIn);
  alpha = colorResult.a;
  return colorResult.rgb;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn) {
  return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn) {
  return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn) {
  return alpha * opacity;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn) {
  return vec3(0);
}

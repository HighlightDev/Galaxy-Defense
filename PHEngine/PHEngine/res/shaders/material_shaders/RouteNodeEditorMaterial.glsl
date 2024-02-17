#version 400

#include "materialCommon.incl"

uniform float opacity;
uniform vec3 color;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn) {
  const vec3 lightVec = normalize(vec3(-0.7, 1, 0));
  return color * dot(normalize(materialIn.WorldNormal), lightVec);
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn) {
  return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn) {
  return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn) {
  return opacity;
};

vec3 GetMaterialNormalMapNormal(in MATERIAL_VS_OUTPUT materialIn) {
  return vec3(0);
}
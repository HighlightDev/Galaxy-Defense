#version 400

#include "materialCommon.incl.glsl"

uniform sampler2D noise;
uniform float timeSec;
uniform float opacity;

float alpha;

vec4 getElectroColor(in MATERIAL_VS_OUTPUT materialIn)
{
  vec2 uv = materialIn.TextureCoordinates.xy;
  uv = uv * 2. -1.;
  float intensity = texture(noise, materialIn.TextureCoordinates.xy + vec2(timeSec * 0.5f, timeSec * 0.5f)).r;

  float t = clamp((uv.x * -uv.x * 0.08) + 0.58, 0., 1.);          
  float y = abs(intensity * -t + uv.y);
    
  float g = pow(y, 0.2);

  vec4 rgba = vec4(1.70, 1.48, 1.78, 1.98);
  rgba = rgba * -g + rgba;      
  rgba = rgba * rgba * rgba;

  return rgba;
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec4 color = getElectroColor(materialIn);
    alpha = color.a;
	return color.rgb;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn)
{
	return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn)
{
	return alpha * opacity;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
	return vec4(0.0);
}
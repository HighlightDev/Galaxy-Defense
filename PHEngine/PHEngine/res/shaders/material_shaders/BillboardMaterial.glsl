#version 400

#include "materialCommon.incl.glsl"

uniform sampler2D albedo;
uniform sampler2D mask;
uniform vec3 transparency_color_filler;
uniform vec3 albedo_custom_color;
uniform int use_mask;
uniform int inverse_y;
uniform int fill_albedo_transparency_with_color;
uniform int use_custom_color_for_albedo;

float alphaValue;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec2 textureCoordinates = inverse_y == 1 ? vec2(materialIn.TextureCoordinates.x, 1.0 - materialIn.TextureCoordinates.y)
                                             : materialIn.TextureCoordinates.xy;

    vec4 sampleColor = texture(albedo, textureCoordinates);
    alphaValue = use_mask == 1 ? texture(mask, textureCoordinates).a : sampleColor.a;
    sampleColor.rgb = fill_albedo_transparency_with_color == 1 && sampleColor.a < 0.1
        ? transparency_color_filler
        : use_custom_color_for_albedo == 1 ? albedo_custom_color : sampleColor.rgb;
    return sampleColor.rgb;
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
    return alphaValue;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
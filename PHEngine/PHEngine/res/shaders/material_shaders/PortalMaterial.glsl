#version 400

#include "materialCommon.incl.glsl"

uniform float timeSec;

vec2 hash(vec2 p)
{
    mat2 m = mat2(15.32, 83.43, 117.38, 289.59);
    return fract(sin(m * p) * 46783.289);
}

float voronoi(vec2 p)
{
    vec2 g = floor(p);
    vec2 f = fract(p);

    float distanceFromPointToCloestFeaturePoint = 1.0;

    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 latticePoint = vec2(x, y);
            float h = distance(latticePoint + hash(g + latticePoint), f);

            distanceFromPointToCloestFeaturePoint = min(distanceFromPointToCloestFeaturePoint, h);
        }
    }

    return 1.0 - sin(distanceFromPointToCloestFeaturePoint);
}

float voronoi_texture(in vec2 uv)
{
    float t = voronoi(uv * 8.0 + vec2(timeSec));
    t *= 1.0 - length(uv * 2.0);

    return t;
}

float fbm(in vec2 uv)
{
    float sum = 0.00;
    float amp = 1.0;

    for (int i = 0; i < 3; ++i) {
        sum += voronoi_texture(uv) * amp;
        uv += uv;
        amp *= 0.8;
    }

    return sum;
}

float alpha = 1.0;

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{

    vec2 uv = (materialIn.TextureCoordinates.xy * 2.0) - 1.0;
    float t = pow(fbm(uv * 0.3), 2.0);
    alpha = 1.0 - smoothstep(0.5, 1.0, length(uv));
    return vec3(t * 2.0, t * 4.0, t * 8.0);
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
    return alpha;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
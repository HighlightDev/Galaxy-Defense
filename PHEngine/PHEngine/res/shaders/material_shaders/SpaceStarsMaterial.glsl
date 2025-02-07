#version 400

#include "materialCommon.incl.glsl"

uniform float timeSec;
uniform vec2 resolution;

#define iterations 15
#define formuparam 0.53

#define volsteps 5 // this param initially was 20, adds density to the world
#define stepsize 0.1

#define zoom 13.900
#define tile 0.550
#define speed 0.0003

#define brightness 0.002
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.800

mat2 rot1 = mat2(cos(1.0), sin(1.0), -sin(1.0), cos(1.0));
mat2 rot2 = mat2(cos(1.3), sin(1.3), -sin(1.3), cos(1.3));

// this shader was taken from https://www.shadertoy.com/view/XlfGRj#
vec3 getSpaceStarsColor(in MATERIAL_VS_OUTPUT materialIn)
{
    // get coords and direction
    vec2 texCoords = materialIn.TextureCoordinates.xy;
    vec2 uv = texCoords - 0.5;
    uv.y *= resolution.y / resolution.x;
    vec3 dir = vec3(uv * zoom, 1.0);
    float time = timeSec * speed + 0.25;

    dir.xz *= rot1;
    dir.xy *= rot2;
    vec3 from = vec3(1.0, 0.5, 0.5);
    from += vec3(time * 2.0, time, -2.0);
    from.xz *= rot1;
    from.xy *= rot2;

    // volumetric rendering
    float s = 0.1, fade = 1.0;
    vec3 v = vec3(0.0);
    for (int r = 0; r < volsteps; r++) {
        vec3 p_varying = from + s * dir * 0.5;
        p_varying = abs(vec3(tile) - mod(p_varying, vec3(tile * 2.0))); // tiling fold
        float pa, a = pa = 0.0;
        for (int i = 0; i < iterations; i++) {
            p_varying = abs(p_varying) / dot(p_varying, p_varying) - formuparam; // the magic formula
            float len = length(p_varying);
            a += abs(len - pa); // absolute sum of average change
            pa = len;
        }
        float dm = max(0.0, darkmatter - a * a * 0.001); // dark matter
        a *= a * a; // add contrast
        float statement = step(7, r);
        fade = (statement * ((1.0 - dm) * fade)) + ((1.0 - statement) * fade);

        v += fade;
        float s_sqr = s * s;
        v += vec3(s, s_sqr, s_sqr * s_sqr) * a * brightness * fade; // coloring based on distance
        fade *= distfading; // distance fading
        s += stepsize;
    }
    float v_len = length(v);
    v = mix(vec3(v_len), v, saturation); // color adjust

    return vec3(v * 0.01);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    return getSpaceStarsColor(materialIn);
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
    return 1.0;
};

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn)
{
    return vec4(0.0);
}
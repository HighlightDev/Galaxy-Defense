#version 440

#include "materialCommon.incl.glsl"

uniform float deltaTimeSec;
uniform sampler2D noise_1;
uniform sampler2D noise_2;

// vec4 CalculateFireColor(in vec2 vTexCoord, in float fFrameTime)
// {
//     vec2 scrollSpeeds = vec2(0.0, -0.5); // Scroll upwards over time
//     vec2 scale1 = vec2(3.0, 1.5);
//     vec2 scale2 = vec2(1.5, 3.0);

//     // Compute animated texture coordinates
//     vec2 animatedTexCoord1 = vTexCoord * scale1 + scrollSpeeds * fFrameTime;
//     vec2 animatedTexCoord2 = vTexCoord * scale2 + scrollSpeeds * fFrameTime * 0.7; // Slower scroll for second layer

//     // Sample noise textures
//     float noiseValue1 = texture(noise_1, animatedTexCoord1).r;
//     float noiseValue2 = texture(noise_2, animatedTexCoord2).r;

//     // Combine noise values
//     float combinedNoise = (noiseValue1 + noiseValue2) * 0.5;

//     // Apply alpha mask to shape the fire (e.g., a simple vertical gradient)
//     float alphaMask = smoothstep(0.0, 1.0, vTexCoord.y); // Makes fire fade towards the top
//     combinedNoise *= alphaMask;

//     // Map noise value to fire colors
//     vec4 fireColor;
//     if (combinedNoise > 0.6) {
//         // Yellow core
//         fireColor = vec4(1.0, 1.0, 0.0, combinedNoise);
//     } else if (combinedNoise > 0.3) {
//         // Orange
//         fireColor = vec4(1.0, 0.5, 0.0, combinedNoise);
//     } else if (combinedNoise > 0.1) {
//         // Red
//         fireColor = vec4(1.0, 0.0, 0.0, combinedNoise);
//     } else {
//         // Transparent
//         fireColor = vec4(0.0, 0.0, 0.0, 0.0);
//     }

//     // Add glow effect using the alpha value
//     fireColor.rgb *= fireColor.a;
//     return fireColor;
// }

// #define BLUE_FLAME

// vec2 hash(vec2 p)
// {
//     p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
//     return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
// }

// float noise(in vec2 p)
// {
//     const float K1 = 0.366025404; // (sqrt(3)-1)/2;
//     const float K2 = 0.211324865; // (3-sqrt(3))/6;

//     vec2 i = floor(p + (p.x + p.y) * K1);

//     vec2 a = p - i + (i.x + i.y) * K2;
//     vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
//     vec2 b = a - o + K2;
//     vec2 c = a - 1.0 + 2.0 * K2;

//     vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

//     vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));

//     return dot(n, vec3(70.0));
// }

// float fbm(vec2 uv)
// {
//     float f;
//     mat2 m = mat2(1.6, 1.2, -1.2, 1.6);
//     f = 0.5000 * noise(uv);
//     uv = m * uv;
//     f += 0.2500 * noise(uv);
//     uv = m * uv;
//     f += 0.1250 * noise(uv);
//     uv = m * uv;
//     f += 0.0625 * noise(uv);
//     uv = m * uv;
//     f = 0.5 + 0.5 * f;
//     return f;
// }

// // no defines, standard redish flames
// //#define BLUE_FLAME
// //#define GREEN_FLAME

// vec4 mainImage(in vec2 vTexCoord, in float fFrameTime)
// {
//     vec2 uv = vTexCoord;
//     vec2 q = uv;
//     q.x *= 1.0;
//     q.y *= 0.8;
//     float strength = floor(q.x + 1.);
//     float T3 = max(3., 1.25 * strength) * fFrameTime;
//     q.x = mod(q.x, 5.) - 0.5;
//     q.y -= 0.25;
//     float n = fbm(strength * q - vec2(0, T3));
//     float c = 1. - 16. * pow(max(0., length(q * vec2(1.8 + q.y * 1.5, .75)) - n * max(0., q.y + .25)), 1.2);
//     //	float c1 = n * c * (1.5-pow(1.25*uv.y,4.));
//     float c1 = n * c * (1.5 - pow(2.50 * uv.y, 4.));
//     c1 = clamp(c1, 0., 1.);

//     vec3 col = vec3(1.5 * c1, 1.5 * c1 * c1 * c1, c1 * c1 * c1 * c1 * c1 * c1);
//     col = mix(
//         col,
//         pow(vec3(1. - clamp(c1, -1., 0.)) * pow(fbm(strength * q * 1.25 - vec2(0, T3)), 2.), vec3(2.)),
//         .75 - (col.x + col.y + col.z) / 3.); // Just added this line!!! :)

// #ifdef BLUE_FLAME
//     col = col.zyx;
// #endif
// #ifdef GREEN_FLAME
//     col = 0.85 * col.yxz;
// #endif

//     float a = c * (1. - pow(uv.y, 3.));
//     return vec4(mix(vec3(0.), col, a), a);
// }

float alpha = 0.0;

float snoise(vec3 uv, float res)
{
    const vec3 s = vec3(1e0, 1e2, 1e3);

    uv *= res;

    vec3 uv0 = floor(mod(uv, res)) * s;
    vec3 uv1 = floor(mod(uv + vec3(1.), res)) * s;

    vec3 f = fract(uv);
    f = f * f * (3.0 - 2.0 * f);

    vec4 v = vec4(uv0.x + uv0.y + uv0.z, uv1.x + uv0.y + uv0.z, uv0.x + uv1.y + uv0.z, uv1.x + uv1.y + uv0.z);

    vec4 r = fract(sin(v * 1e-1) * 1e3);
    float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);

    r = fract(sin((v + uv1.z - uv0.z) * 1e-1) * 1e3);
    float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);

    return mix(r0, r1, f.z) * 2. - 1.;
}

vec4 mainImage(in vec2 texCoords, in float deltaTimeSec)
{
    vec2 p = -.5 + texCoords;
    p.x *= 1.0;
    // iResolution.x/iResolution.y;

    float color = 3.0 - (3. * length(2. * p));

    vec3 coord = vec3(atan(p.x, p.y) / 6.2832 + .5, length(p) * .4, .5);

    for (int i = 1; i <= 7; i++) {
        float power = pow(2.0, float(i));
        color += (1.5 / power) * snoise(coord + vec3(0., -deltaTimeSec * .05, deltaTimeSec * .01), power * 16.);
    }

    float r = color;
    float g = pow(max(color, 0.), 2.) * 0.4;
    float b = pow(max(color, 0.), 3.) * 0.15;

    return vec4(b, g, r, 1.0);
}

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn)
{
    vec4 fireColorAlpha = mainImage(materialIn.TextureCoordinates.xy, deltaTimeSec * 10);
    alpha = fireColorAlpha.a;
    return fireColorAlpha.rgb;
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
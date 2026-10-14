#version 440 core

#include "materialCommon.incl.glsl"

// Procedural space background billboard. Drives MainMenuLevel's full-screen
// backdrop and is reused by CombatLevel (with the planet/ring switched off).
// Layers from back to front: volumetric starfield adapted from
// SpaceStarsMaterial (Shadertoy XlfGRj), three drifting nebula blobs, a
// grid-hash sparkle layer, and (optionally) a sphere-shaded planet with halo
// and tilted ring. All visual coefficients are randomised on the host side
// per level load — see ProceduralSpaceBackgroundBuilder.cpp.
//
// Uniforms:
//   timeSec         — accumulating delta-time (bound via GT_DeltaSec)
//   aspectRatio     — window aspect (w/h), set from C++ at level start
//   randomSeed      — per-load offset that shifts star + sparkle patterns
//                     so each launch shows a distinct cosmos
//   planetCenter    — vec3, only .xy is used; planet centre in UV (0..1)
//   planetRadius    — disk radius in UV vertical units
//   planetHue       — base planet colour (mid-shade)
//   planetHighlight — rim/highlight colour
//   nebulaHue       — primary nebula tint
//   nebulaTintB     — secondary nebula tint
//   nebulaTintC     — tertiary nebula tint
//   planetEnabled   — 0/1, draw the planet at all
//   ringEnabled     — 0/1, draw the orbital ring

uniform float timeSec;
uniform float aspectRatio;
uniform float randomSeed;
uniform vec3 planetCenter;
uniform float planetRadius;
uniform vec3 planetHue;
uniform vec3 planetHighlight;
uniform vec3 nebulaHue;
uniform vec3 nebulaTintB;
uniform vec3 nebulaTintC;
uniform int planetEnabled;
uniform int ringEnabled;

// ---------------- Volumetric starfield (XlfGRj, slowed down) ----------------

#define STAR_ITER 15
#define STAR_FORMUPARAM 0.53
#define STAR_VOLSTEPS 5
#define STAR_STEPSIZE 0.1
#define STAR_ZOOM 13.9
#define STAR_TILE 0.55
#define STAR_SPEED 0.00012
#define STAR_BRIGHTNESS 0.0024
#define STAR_DARKMATTER 0.30
#define STAR_DISTFADING 0.73
#define STAR_SATURATION 0.85

vec3 volumetricStars(vec2 uv01)
{
    mat2 rot1 = mat2(cos(1.0), sin(1.0), -sin(1.0), cos(1.0));
    mat2 rot2 = mat2(cos(1.3), sin(1.3), -sin(1.3), cos(1.3));

    vec2 uv = uv01 - 0.5;
    uv.x *= aspectRatio;
    vec3 dir = vec3(uv * STAR_ZOOM, 1.0);
    float time = timeSec * STAR_SPEED + 0.25;

    dir.xz *= rot1;
    dir.xy *= rot2;

    vec3 from = vec3(1.0, 0.5, 0.5);
    from += vec3(time * 0.6, time * 0.4, -2.0);
    // randomSeed shifts the volume origin so consecutive level loads sample
    // a different slice of the procedural galaxy — same algorithm, fresh view.
    from += vec3(randomSeed * 17.13, randomSeed * 23.71, randomSeed * 31.59);
    from.xz *= rot1;
    from.xy *= rot2;

    float s = 0.1;
    float fade = 1.0;
    vec3 v = vec3(0.0);
    for (int r = 0; r < STAR_VOLSTEPS; r++) {
        vec3 p = from + s * dir * 0.5;
        p = abs(vec3(STAR_TILE) - mod(p, vec3(STAR_TILE * 2.0)));
        float pa = 0.0;
        float a = 0.0;
        for (int i = 0; i < STAR_ITER; i++) {
            p = abs(p) / dot(p, p) - STAR_FORMUPARAM;
            float len = length(p);
            a += abs(len - pa);
            pa = len;
        }
        float dm = max(0.0, STAR_DARKMATTER - a * a * 0.001);
        a *= a * a;
        float st = step(7.0, float(r));
        fade = (st * ((1.0 - dm) * fade)) + ((1.0 - st) * fade);
        v += fade;
        float s2 = s * s;
        v += vec3(s * 0.55, s2 * 0.45, s2 * s2 * 0.40) * a * STAR_BRIGHTNESS * fade;
        fade *= STAR_DISTFADING;
        s += STAR_STEPSIZE;
    }
    float vl = length(v);
    v = mix(vec3(vl), v, STAR_SATURATION);
    return v * 0.014;
}

// ---------------- Sparkle stars (cheap grid hash + twinkle) ----------------

float hash21(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

vec3 sparkleStars(vec2 uv)
{
    vec2 grid = vec2(50.0 * aspectRatio, 50.0);
    // randomSeed shifts every cell into a new hash bucket — different stars
    // appear in different places each load while still living on the same
    // pixel grid. The 100x scaling keeps adjacent seeds well separated.
    vec2 cellId = floor(uv * grid) + vec2(randomSeed * 113.0, randomSeed * 197.0);
    vec2 cellUv = fract(uv * grid) - 0.5;
    float h = hash21(cellId);

    float threshold = 0.985;
    float t = max(0.0, h - threshold) / (1.0 - threshold);
    if (t <= 0.0)
        return vec3(0.0);

    vec2 offset = vec2(hash21(cellId + 7.0), hash21(cellId + 13.0)) * 0.6 - 0.3;
    float d = length(cellUv - offset);
    float radius = 0.025 + t * 0.04;
    float core = smoothstep(radius, 0.0, d);
    float glow = smoothstep(radius * 4.0, 0.0, d) * 0.25;
    float twinkle = 0.45 + 0.55 * sin(timeSec * 1.7 + h * 30.0);
    vec3 tint = mix(vec3(0.80, 0.92, 1.00), vec3(1.0, 0.95, 0.78), hash21(cellId + 3.0));
    return tint * (core + glow) * twinkle * 0.9;
}

// ---------------- Nebula glow (a handful of large soft blobs) ----------------

vec3 nebulaGlow(vec2 centered)
{
    float t = timeSec * 0.03;
    vec2 c1 = vec2(-0.45, 0.22) + 0.05 * vec2(sin(t), cos(t * 1.3));
    vec2 c2 = vec2(0.35, -0.18) + 0.05 * vec2(cos(t * 0.8), sin(t));
    vec2 c3 = vec2(0.10, 0.40) + 0.04 * vec2(sin(t * 1.5), cos(t * 0.7));

    float n1 = exp(-dot(centered - c1, centered - c1) * 5.0);
    float n2 = exp(-dot(centered - c2, centered - c2) * 7.0);
    float n3 = exp(-dot(centered - c3, centered - c3) * 9.0);

    return nebulaHue * n1 * 0.38 + nebulaTintB * n2 * 0.22 + nebulaTintC * n3 * 0.18;
}

// ---------------- Planet (sphere shading + halo + tilted ring) ----------------

// returns the planet/ring color and writes 1.0 into bodyMask if the fragment is
// inside the opaque planet disk (so the caller can skip starfield/nebula there).
vec3 planet(vec2 uv01, out float bodyMask)
{
    bodyMask = 0.0;
    if (planetEnabled == 0)
        return vec3(0.0);

    // Slow horizontal sway so the planet feels alive without being distracting.
    vec2 pc = planetCenter.xy + vec2(sin(timeSec * 0.04) * 0.012, 0.0);

    vec2 toP = uv01 - pc;
    toP.x *= aspectRatio;
    float d = length(toP);

    vec3 col = vec3(0.0);

    // Atmospheric halo
    float halo = exp(-(d - planetRadius) * 7.0) * smoothstep(planetRadius * 1.7, planetRadius, d);
    halo = max(halo, 0.0);
    col += planetHighlight * halo * 0.35;

    // Tilted ring around the planet
    if (ringEnabled == 1) {
        float ringTilt = -0.14;
        float cs = cos(ringTilt);
        float sn = sin(ringTilt);
        vec2 rp = vec2(cs * toP.x + sn * toP.y, -sn * toP.x + cs * toP.y);
        float ringMajor = planetRadius * 1.55;
        float ringMinor = planetRadius * 0.30;
        float ringDist = abs(length(vec2(rp.x / ringMajor, rp.y / ringMinor)) - 1.0);
        float ringMask = smoothstep(0.05, 0.0, ringDist) * 0.55;
        // Front part of the ring (below the planet equator in rotated space) is
        // dimmed a bit so the body still reads as solid.
        ringMask *= mix(1.0, 0.55, smoothstep(0.0, planetRadius * 0.5, rp.y));
        col += planetHighlight * ringMask;
    }

    if (d < planetRadius) {
        // Sphere normal (front hemisphere only)
        vec2 nrm = toP / planetRadius;
        float z = sqrt(max(0.0, 1.0 - dot(nrm, nrm)));
        vec3 N = vec3(nrm, z);
        vec3 L = normalize(vec3(0.55, 0.32, 0.78));
        float ndl = max(0.0, dot(N, L));

        // Subtle horizontal bands across the surface
        float band = 0.5 + 0.5 * sin(nrm.y * 12.0 + sin(nrm.x * 3.0) * 1.5);

        vec3 baseCol = mix(planetHue * 0.18, planetHue, ndl);
        baseCol = mix(baseCol, planetHighlight, ndl * 0.40 + band * 0.06 - 0.04);
        // Terminator fall-off so the night side is properly dark
        baseCol *= smoothstep(-0.15, 0.55, ndl);

        col = baseCol;
        bodyMask = 1.0;
    }

    return col;
}

// ---------------- Material entry points ----------------

vec3 GetMaterialAlbedo(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    vec2 uv = materialIn.TextureCoordinates.xy;
    vec2 centered = uv - 0.5;
    centered.x *= aspectRatio;

    vec3 stars = volumetricStars(uv) * 1.45;
    vec3 neb = nebulaGlow(centered);
    vec3 sparks = sparkleStars(uv);

    float bodyMask = 0.0;
    vec3 pln = planet(uv, bodyMask);

    vec3 col;
    if (bodyMask > 0.5) {
        col = pln; // opaque planet body overrides background layers
    } else {
        col = stars + neb + sparks + pln;
    }

    // Gentle vignette so the corners of the screen don't blow out the chrome
    float vig = smoothstep(1.15, 0.55, length(centered));
    col *= mix(0.55, 1.0, vig);

    // A whisper of ambient blue so pure-black pixels still have life
    col += vec3(0.006, 0.010, 0.018);

    return col;
}

vec2 GetMaterialMetallicRoughness(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec2(0);
}

float GetMaterialAmbientOcclusion(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 0.0;
}

float GetMaterialAlphaMask(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return 1.0;
}

vec3 GetMaterialWorldNormal(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec3(0);
}

vec4 GetMaterialEmission(in MATERIAL_VS_OUTPUT materialIn, in FLAT_MATERIAL_VS_OUTPUT flatMaterialIn)
{
    return vec4(0.0);
}

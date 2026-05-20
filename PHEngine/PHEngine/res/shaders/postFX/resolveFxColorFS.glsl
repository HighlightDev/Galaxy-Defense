#version 440 core

#define HDR_ON
#define BLOOM_ON
#define HDR_EXPOSURE 1.5
layout(location = 0) out vec4 FragColor;

uniform sampler2D sceneColorTexture;
uniform sampler2D bloomColorTexture;

// 0 - opaque result (resolve 3D scene), 1 - alpha is taken from the source texture (resolve GUI).
uniform float resolveAlphaFromSource = 0.0;

in vec2 vs_out_texCoords;

#if defined(HDR_ON) && defined(BLOOM_ON)
vec3 resolveHdrAndBloom()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(sceneColorTexture, vs_out_texCoords).rgb;
    vec3 bloomColor = texture(bloomColorTexture, vs_out_texCoords).rgb;
    hdrColor += bloomColor;
    // tone mapping
    return (vec3(1.0) - exp(-hdrColor * HDR_EXPOSURE));
}
#endif

#ifdef BLOOM_ON
vec3 resolveBloom()
{
    const float gamma = 2.2;
    vec3 sceneColor = texture(sceneColorTexture, vs_out_texCoords).rgb;
    vec3 bloomColor = texture(bloomColorTexture, vs_out_texCoords).rgb;
    sceneColor += bloomColor;
    return sceneColor;
}
#endif

#ifdef HDR_ON
vec3 resolveHDR()
{
    vec3 sceneColor = texture(sceneColorTexture, vs_out_texCoords).rgb;
    return (vec3(1.0) - exp(-sceneColor * HDR_EXPOSURE));
}
#endif

vec4 resolveBloomColor()
{
#if defined(HDR_ON) && defined(BLOOM_ON)
    return vec4(resolveHdrAndBloom(), 1.0);
#endif
#if defined(BLOOM_ON)
    return vec4(resolveBloom(), 1.0);
#endif
#if defined(HDR_ON)
    return vec4(resolveHDR(), 1.0);
#else
    return vec4(texture(sceneColorTexture, vs_out_texCoords).rgb, 1.0);
#endif
}

void main()
{
    vec4 resolvedColor = resolveBloomColor();
    float sourceAlpha = texture(sceneColorTexture, vs_out_texCoords).a;
    resolvedColor.a = mix(1.0, sourceAlpha, resolveAlphaFromSource);
    FragColor = resolvedColor;
}
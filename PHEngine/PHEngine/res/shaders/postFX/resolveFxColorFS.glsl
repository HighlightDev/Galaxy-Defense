#version 400

#define HDR_ON
#define BLOOM_ON
#define HDR_EXPOSURE 1.5
layout(location = 0) out vec4 FragColor;

uniform sampler2D sceneColorTexture;
uniform sampler2D bloomColorTexture;

in vec2 vs_out_texCoords;

#if defined(HDR_ON) && defined(BLOOM_ON)
	vec3 resolveHdrAndBloom()
	{
		const float gamma = 2.2;
	  	vec3 hdrColor = texture(sceneColorTexture, vs_out_texCoords).rgb;
	  	vec3 bloomColor = texture(bloomColorTexture, vs_out_texCoords).rgb;
	  	// also gamma correct while we're at it
	  	bloomColor = pow(bloomColor, vec3(1.0 / gamma));
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
		// also gamma correct while we're at it
		bloomColor = pow(bloomColor, vec3(1.0 / gamma));
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

vec4 resolveBloomColor() {
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

void main() { FragColor = resolveBloomColor(); }
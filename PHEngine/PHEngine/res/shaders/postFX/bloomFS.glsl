#version 400

const float weights[5] = float[] (0.204164,0.180174,0.123832,0.066282,0.027631);
#define BLUR_WIDTH 5
#define bloomThreshold 0.45
layout (location = 0) out vec4 FragColor;

uniform sampler2D sceneColorTexture;
uniform sampler2D bluredColorTexture;

subroutine vec4 executePostFx();
subroutine uniform executePostFx execBloomFx;

in vec2 vs_out_texCoords;

const vec3 luma = vec3(0.2126, 0.7152, 0.0722);

subroutine(executePostFx)
vec4 extractBrightParts()
{
	vec4 sceneColor = texture(sceneColorTexture, vs_out_texCoords);
    return sceneColor * step(bloomThreshold, dot(sceneColor.rgb, luma));
}

subroutine(executePostFx)
vec4 runVerticalBlur()
{
    float texel_dy = 1.0 / textureSize(sceneColorTexture, 0).y;
	vec4 sum = texture(sceneColorTexture, vs_out_texCoords) * weights[0];
	for( int i = 1; i < BLUR_WIDTH; ++i)
	{
		sum += texture( sceneColorTexture, vs_out_texCoords + vec2(0.0, i * texel_dy )) * weights[i];
		sum += texture( sceneColorTexture, vs_out_texCoords - vec2(0.0, i * texel_dy )) * weights[i];
	}
	return sum;
}

subroutine(executePostFx)
vec4 runHorizontalBlur()
{
    float texel_dx = 1.0 / textureSize(sceneColorTexture, 0).x;
	vec4 sum = texture(sceneColorTexture, vs_out_texCoords) * weights[0];
	for( int i = 1; i < BLUR_WIDTH; ++i)
	{
		sum += texture( sceneColorTexture, vs_out_texCoords + vec2(i * texel_dx , 0.0)) * weights[i];
		sum += texture( sceneColorTexture, vs_out_texCoords - vec2(i * texel_dx , 0.0)) * weights[i];
	}
	return sum;
}

void main()
{
    FragColor = execBloomFx();
}
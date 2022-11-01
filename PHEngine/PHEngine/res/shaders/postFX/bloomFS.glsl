#version 400

layout (location = 0) out vec4 FragColor;

uniform sampler2D sceneColorTexture;

subroutine vec4 executePostFx();
subroutine uniform executePostFx execBloomFx;

in vec2 vs_out_texCoords;

const float bloomThreshold = 0.55;
const vec3 luma = vec3(0.2126, 0.7152, 0.0722);

subroutine(executePostFx)
vec4 extractBrightParts()
{
	vec4 sceneColor = texture(sceneColorTexture, vs_out_texCoords);
    return sceneColor * step(bloomThreshold, dot(sceneColor.rgb, luma));
}

void main()
{   
    FragColor = execBloomFx();
}
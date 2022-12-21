#version 400

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 texCoord;

out vec2 texCoords;

uniform vec2 translation;
uniform vec2 scale;

vec3 fromNdcToTexSpace(in vec3 in_vertex)
{
	return ((in_vertex * 0.5) + 0.5);
}

vec3 fromTexSpaceToNdc(in vec3 in_vertex)
{
	return ((in_vertex * 2.0) - 1.0);
}

void main(void)
{
	texCoords = vec2(texCoord.x, 1 - texCoord.y);
	vec3 texSpaceCoords = fromNdcToTexSpace(vertex);
	texSpaceCoords = (texSpaceCoords * vec3(scale, 1.0)) + vec3(translation, 0.0);
	vec3 ndcSpaceCoords = fromTexSpaceToNdc(texSpaceCoords);
	gl_Position = vec4(ndcSpaceCoords, 1.0);
}
#version 400

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 texCoord;

out vec2 texCoords;

uniform vec2 translation;
uniform vec2 scale;
uniform float rotationRadians = 0.0;
uniform int isFlipped;

mat3 rollMatrix(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    
    return mat3(c,   -s,  0.0,
                s,    c,  0.0,
                0.0, 0.0, 1.0);
}

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
	texCoords = vec2((1 - texCoord.x) * isFlipped + ((1 - isFlipped) * texCoord.x), 1 - texCoord.y);
	vec3 rotatedVertex = rollMatrix(rotationRadians) * vertex;
	vec3 texSpaceCoords = fromNdcToTexSpace(rotatedVertex);
	texSpaceCoords = (texSpaceCoords * vec3(scale, 1.0)) + vec3(translation, 0.0);
	vec3 ndcSpaceCoords = fromTexSpaceToNdc(texSpaceCoords);
	gl_Position = vec4(ndcSpaceCoords, 1.0);
}
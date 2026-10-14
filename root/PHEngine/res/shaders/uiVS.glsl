#version 440 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 2) in vec2 texCoord;

out vec2 texCoords;

uniform vec2 translation;
uniform vec2 scale;
uniform float rotationRadians = 0.0;
uniform int isFlipped;

mat3 rollMatrix(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return mat3(c, -s, 0.0, s, c, 0.0, 0.0, 0.0, 1.0);
}

vec2 fromNdcToTexSpace(in vec2 in_vertex)
{
    return ((in_vertex * 0.5) + 0.5);
}

vec2 fromTexSpaceToNdc(in vec2 in_vertex)
{
    return ((in_vertex * 2.0) - 1.0);
}

void main(void)
{
    texCoords = vec2((1 - texCoord.x) * isFlipped + ((1 - isFlipped) * texCoord.x), 1 - texCoord.y);
    vec2 rotatedVertex = (rollMatrix(rotationRadians) * vec3(vertexPosition, 1.0)).xy;
    vec2 texSpaceCoords = fromNdcToTexSpace(rotatedVertex);
    texSpaceCoords = (texSpaceCoords * scale) + translation;
    vec2 ndcSpaceCoords = fromTexSpaceToNdc(texSpaceCoords);
    gl_Position = vec4(ndcSpaceCoords, 0.0, 1.0);
}
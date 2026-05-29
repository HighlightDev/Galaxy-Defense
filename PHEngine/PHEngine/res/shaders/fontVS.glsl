#version 440 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 2) in vec2 texCoord;
layout(location = 7) in vec3 vertexColor;

out vec2 texCoords;
out vec3 interpolatedVertexColor;

uniform vec2 position;

void main(void)
{
    vec2 ndcPosition = position * vec2(2.0, -2.0);
    texCoords = texCoord;
    interpolatedVertexColor = vertexColor;
    gl_Position = vec4(vertexPosition + ndcPosition, 0.0, 1.0);
}
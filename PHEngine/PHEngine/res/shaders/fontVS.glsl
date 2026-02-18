#version 440 core

layout(location = 0) in vec2 vertex;
layout(location = 2) in vec2 texCoord;

out vec2 texCoords;

uniform vec2 position;

void main(void)
{
    vec2 ndcPosition = position * vec2(2.0, -2.0);
    texCoords = texCoord;
    gl_Position = vec4(vertex + ndcPosition, 0.0, 1.0);
}
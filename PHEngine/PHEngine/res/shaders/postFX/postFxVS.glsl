#version 440 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 2) in vec2 texCoords;

out vec2 vs_out_texCoords;

void main()
{
    vs_out_texCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
#version 400

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 vs_out_texCoords;

void main()
{   
    vs_out_texCoords = vec2(texCoords.x, 1.0 - texCoords.y);
    gl_Position = vec4(position, 1.0);
}
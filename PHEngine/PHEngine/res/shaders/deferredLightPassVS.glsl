#version 400

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out VS_OUT
{
	vec2 tex_coords;
} vs_out;

void main(void)
{
	vs_out.tex_coords = vec2(texCoord.x, 1 - texCoord.y);
	gl_Position = vec4(position, 1.0);
}

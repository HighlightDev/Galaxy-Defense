#version 400

layout (location = 0) in vec2 vertex;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;
layout (location = 8) in vec2 offset;

out vec2 texCoords;
out vec3 out_color;

void main(void)
{
	vec2 ndcPosition = offset * vec2(2.0, -2.0);
	texCoords = texCoord;
	out_color = color;
	gl_Position = vec4(vertex + ndcPosition, 0.0, 1.0);
}
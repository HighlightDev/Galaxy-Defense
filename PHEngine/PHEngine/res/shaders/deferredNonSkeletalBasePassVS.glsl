#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;

out VS_OUT
{
	vec3 world_normal;
	vec3 world_tangent;
	vec3 world_bitangent;
	vec3 world_position;
	vec2 tex_coords;
} vertex_shader_result;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	vertex_shader_result.world_normal = (worldMatrix * vec4(normal, 0.0)).xyz;
	vertex_shader_result.world_tangent = (worldMatrix * vec4(tangent, 0.0)).xyz;
	vertex_shader_result.world_bitangent = (worldMatrix * vec4(bitangent, 0.0)).xyz;
	vertex_shader_result.tex_coords = texCoord;

	vec4 worldPosition = worldMatrix * vec4(position, 1.0);

	vertex_shader_result.world_position = worldPosition.xyz;

	gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
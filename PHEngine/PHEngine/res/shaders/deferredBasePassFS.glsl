#version 400

layout (location = 0) out vec3 gBuffer_Position;
layout (location = 1) out vec3 gBuffer_Normal;
layout (location = 2) out vec4 gBuffer_AlbedoNSpecular;

in VS_OUT
{
	vec3 world_normal;
	vec3 world_tangent;
	vec3 world_bitangent;
	vec3 world_position;
	vec2 tex_coords;
} vertex_shader_result;

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

void main()
{
	vec3 albedoColor = texture(albedo, vertex_shader_result.tex_coords).rgb;
	float specularFactor = texture(specularMap, vertex_shader_result.tex_coords).r;

	vec3 normalFromNM = (texture(normalMap, vertex_shader_result.tex_coords).xyz * 2.0) - 1.0;

	vec3 normal = normalize(vertex_shader_result.world_normal);
	vec3 tangent = normalize(vertex_shader_result.world_tangent);
	vec3 bitangent = normalize(vertex_shader_result.world_bitangent);

	mat3 tangentToWorld = mat3(tangent, bitangent, normal);

	normalFromNM = tangentToWorld * normalFromNM;
	
	gBuffer_Position = vertex_shader_result.world_position;
	gBuffer_Normal = normalize(normalFromNM);
	gBuffer_AlbedoNSpecular = vec4(albedoColor, specularFactor);
}
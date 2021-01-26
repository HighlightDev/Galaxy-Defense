#version 400

#define CubemapFaces 6
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;


uniform mat4 shadowViewMatrices[CubemapFaces];
uniform mat4 shadowProjectionMatrices[CubemapFaces];

out vec3 FragPos;

void main(void)
{
	for (int face = 0; face < CubemapFaces; ++face)
	{
		gl_Layer = face;
		for (int i = 0; i < 3; ++i)
		{
			FragPos = gl_in[i].gl_Position.xyz;
			gl_Position =  shadowProjectionMatrices[face] * shadowViewMatrices[face] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}
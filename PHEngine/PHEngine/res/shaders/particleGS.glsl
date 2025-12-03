#version 440

#include "materialCommon.incl.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140) uniform Matrices
{
    mat4 worldMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

in vec2 vs_out_rotation_size[];
in vec4 vs_out_particle_color[];

in MATERIAL_VS_OUTPUT VsOutput[];

out vec4 out_color;
out MATERIAL_VS_OUTPUT GsOutput;

mat4 rollMatrix(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return mat4(c, -s, 0.0, 0.0, s, c, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

void main()
{
    vec2 rotation_size = vs_out_rotation_size[0];

    out_color = vs_out_particle_color[0];

    mat4 rotationMatrix = rollMatrix(rotation_size.x);

    vec4 rotatedVertex1 = rotationMatrix * vec4(-rotation_size.y, rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex2 = rotationMatrix * vec4(-rotation_size.y, -rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex3 = rotationMatrix * vec4(rotation_size.y, rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex4 = rotationMatrix * vec4(rotation_size.y, -rotation_size.y, 0.0, 1.0);

    vec3 texCoordsVertex1 = vec3(0.0, 1.0, 0.0);
    vec3 texCoordsVertex2 = vec3(0.0, 0.0, 0.0);
    vec3 texCoordsVertex3 = vec3(1.0, 1.0, 0.0);
    vec3 texCoordsVertex4 = vec3(1.0, 0.0, 0.0);

    gl_Position = projectionMatrix * (rotatedVertex1 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex1;
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex2 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex2;
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex3 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex3;
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex4 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex4;
    EmitVertex();

    EndPrimitive();
}
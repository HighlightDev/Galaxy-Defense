#version 400

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projectionMatrix;
uniform float extent;

out vec2 gs_out_texCoords;

void main()
{
    vec4 vertex1 = vec4(-extent, extent, 0.0, 1.0);
    vec4 vertex2 = vec4(-extent, -extent, 0.0, 1.0);
    vec4 vertex3 = vec4(extent, extent, 0.0, 1.0);
    vec4 vertex4 = vec4(extent, -extent, 0.0, 1.0);

    vec2 texCoordsVertex1 = vec2(0.0, 1.0);
    vec2 texCoordsVertex2 = vec2(0.0, 0.0);
    vec2 texCoordsVertex3 = vec2(1.0, 1.0);
    vec2 texCoordsVertex4 = vec2(1.0, 0.0);
    
    gl_Position = projectionMatrix * (vertex1 + gl_in[0].gl_Position);
    gs_out_texCoords = texCoordsVertex1;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex2 + gl_in[0].gl_Position);
    gs_out_texCoords = texCoordsVertex2;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex3 + gl_in[0].gl_Position);
    gs_out_texCoords = texCoordsVertex3;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex4 + gl_in[0].gl_Position);
    gs_out_texCoords = texCoordsVertex4;
    EmitVertex();

    EndPrimitive();
}
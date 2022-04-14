#version 400

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projectionMatrix;

in vec4 vs_out_color[];
in float vs_out_particle_size[];

out vec4 out_color;

void main()
{
    mat4 wvpMatrix = projectionMatrix;
    vec4 position = gl_in[0].gl_Position;
    float particleSize = vs_out_particle_size[0];
    out_color = vs_out_color[0];
    
    gl_Position = projectionMatrix * (vec4(-particleSize, particleSize, 0.0, 1.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (vec4(-particleSize, -particleSize, 0.0, 1.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (vec4(particleSize, particleSize, 0.0, 1.0) + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (vec4(particleSize, -particleSize, 0.0, 1.0) + gl_in[0].gl_Position);
    EmitVertex();

    EndPrimitive();
}


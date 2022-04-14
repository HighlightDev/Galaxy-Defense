#version 400

layout (location = 0) in vec3 position;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

uniform vec4 color;
uniform float particleSize;

out vec4 vs_out_color;
out float vs_out_particle_size;

void main()
{
    vs_out_color = color;
    vs_out_particle_size = particleSize;
    gl_Position = viewMatrix * worldMatrix * vec4(position, 1.0);
}


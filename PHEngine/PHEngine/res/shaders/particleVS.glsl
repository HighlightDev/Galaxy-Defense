#version 400

layout (location = 0) in vec3 position;
layout (location = 8) in vec3 relative_translation;
layout (location = 9) in vec2 relative_rotation_size;
layout (location = 10) in vec4 particle_color;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

out vec2 vs_out_rotation_size;
out vec4 vs_out_particle_color;

void main()
{   
    vs_out_particle_color = particle_color;
    vs_out_rotation_size = relative_rotation_size;
    gl_Position = viewMatrix * worldMatrix * vec4(relative_translation, 1.0);
}


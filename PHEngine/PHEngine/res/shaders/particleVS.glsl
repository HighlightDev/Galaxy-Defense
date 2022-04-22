#version 400

out vec2 vs_out_rotation_size;
out vec4 vs_out_particle_color;

void main()
{   
    vs_out_particle_color = GetCustom_2();
    vs_out_rotation_size = GetCustom_1();
    gl_Position = viewMatrix * worldMatrix * vec4(GetCustom_0(), 1.0);
}


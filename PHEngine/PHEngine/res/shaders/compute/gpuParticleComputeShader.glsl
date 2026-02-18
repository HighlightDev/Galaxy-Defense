#version 440 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer Pos
{
    vec4 Positions[];
};

const vec3 G = vec3(0, -9.8, 0);
const float DT = 0.009;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    Positions[index] += vec4(G * DT, 0.0);
}
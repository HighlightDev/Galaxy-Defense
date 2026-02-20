#version 440 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer ParticlePositionBuffer
{
    vec4 ParticlePositions[];
};

layout(std430, binding = 1) buffer ColorBuffer
{
    vec4 Colors[];
};

layout(std430, binding = 2) buffer AliveCounterBuffer
{
    uint AliveCounter;
};

uniform float particleLifetime;
uniform float deltaTimeSec;

const vec3 G = vec3(0, -9.8, 0);

void main()
{
    uint index = gl_GlobalInvocationID.x;
    float currentLifeDuration = ParticlePositions[index].w;
    currentLifeDuration += deltaTimeSec;
    bool particleAlive = currentLifeDuration < particleLifetime;
    if (particleAlive) {
        atomicAdd(AliveCounter, 1);
    }

    ParticlePositions[index] += vec4(G * deltaTimeSec, deltaTimeSec);
    Colors[index] += vec4(deltaTimeSec, 0, 0, 0);
}
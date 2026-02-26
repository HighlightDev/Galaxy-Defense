#version 440 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer ParticlePositionBuffer
{
    vec4 ParticlePositions[];
};

layout(std430, binding = 1) buffer ParticleVelocityBuffer
{
    vec4 ParticleVelocities[];
};

layout(std430, binding = 2) buffer ParticleInitialVelocityBuffer
{
    vec4 ParticleInitialVelocities[];
};

layout(std430, binding = 3) buffer ColorBuffer
{
    vec4 Colors[];
};

layout(std430, binding = 4) buffer RotationAndSizeBuffer
{
    vec2 RotationAndSizes[]; // x - rotation and y - size
};

layout(std430, binding = 5) buffer AliveCounterBuffer
{
    uint AliveCounter;
};

uniform float particleLifetime;
uniform float deltaTimeSec;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    float currentLifeDuration = ParticlePositions[index].w;
    bool particleAlive = currentLifeDuration < particleLifetime;
    if (particleAlive) {
        atomicAdd(AliveCounter, 1);
    }

    float particleLifeFactor = currentLifeDuration / particleLifetime; // [0, 1]
    vec3 updatedColor = updateColor(Colors[index].rgb, particleLifeFactor, deltaTimeSec);
    Colors[index] = vec4(updatedColor, 1.0);

    vec3 initialVelocity = ParticleInitialVelocities[index].xyz;
    vec3 currentVelocity = ParticleVelocities[index].xyz;
    vec3 updatedVelocity = updateVelocity(currentVelocity, particleLifeFactor, deltaTimeSec);
    ParticleVelocities[index] = vec4(updatedVelocity, 0.0);

    vec2 updatedRotationAndSize = updateRotationAndSize(RotationAndSizes[index], particleLifeFactor, deltaTimeSec);
    RotationAndSizes[index] = updatedRotationAndSize;

    const float particleSpeed = 15.0f;
    ParticlePositions[index] += vec4(normalize(initialVelocity + updatedVelocity) * deltaTimeSec * particleSpeed, deltaTimeSec);
}
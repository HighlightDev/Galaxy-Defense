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

layout(std430, binding = 5) buffer ParticleLifetimeBuffer
{
    float ParticleLifetimes[];
};

layout(std430, binding = 6) buffer AliveCounterBuffer
{
    uint AliveCounter;
};

uniform float deltaTimeSec;
uniform bool mIsEndlessRespawnEnabled;

float random(in vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    uint index = gl_GlobalInvocationID.x;
    float maxLifetime = ParticleLifetimes[index];
    vec4 currentPositionAndAge = ParticlePositions[index];
    float currentAge = currentPositionAndAge.w;
    bool particleAlive = currentAge < maxLifetime;

    float particleLifeFactor = currentAge / maxLifetime; // [0, 1]

    vec3 initialVelocity, initialColor, position;
    vec2 initialRotationAndSize;
    float newAge;

    if (!particleAlive && mIsEndlessRespawnEnabled) {
        atomicAdd(AliveCounter, 1);
        vec3 invokeId = vec3(gl_GlobalInvocationID);
        initialVelocity = resetVelocity(deltaTimeSec, invokeId);
        initialColor = resetColor(deltaTimeSec, invokeId);
        initialRotationAndSize = resetRotationAndSize(deltaTimeSec, invokeId);
        newAge = resetLifeTime(deltaTimeSec, invokeId);
        // Respawn at the component's local origin; worldMatrix is applied later in the vertex shader.
        position = vec3(0.0);
        // Update initial velocity so next-frame updateVelocity gets the correct direction
        ParticleInitialVelocities[index] = vec4(initialVelocity, 0.0);
    } else if (particleAlive) {
        atomicAdd(AliveCounter, 1);
        initialVelocity = updateVelocity(
            ParticleInitialVelocities[index].xyz, ParticleVelocities[index].xyz, particleLifeFactor, deltaTimeSec);
        initialColor = updateColor(Colors[index].rgb, particleLifeFactor, deltaTimeSec);
        initialRotationAndSize = updateRotationAndSize(RotationAndSizes[index], particleLifeFactor, deltaTimeSec);
        newAge = updateLifeTime(currentAge, deltaTimeSec);
        position = currentPositionAndAge.xyz;
    } else {
        // Particle is dead and respawn is disabled — leave buffers unchanged.
        // Without this return, local variables above are undefined per GLSL spec.
        // On Windows drivers they contain garbage (e.g. +Inf), causing full-screen artifacts.
        return;
    }

    Colors[index] = vec4(initialColor, 1.0);
    RotationAndSizes[index] = initialRotationAndSize;
    ParticleVelocities[index] = vec4(initialVelocity, 0.0);

    position += initialVelocity * deltaTimeSec;
    ParticlePositions[index] = vec4(position, newAge);
}
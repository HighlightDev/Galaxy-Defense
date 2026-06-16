#version 440 core

layout(std430, binding = 0) buffer Pos
{
    vec4 Positions[];
};

layout(std430, binding = 1) buffer Vel
{
    vec4 Velocities[];
};

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

#define gravityBomb1 vec3(50, 100, 50)
#define gravityBomb2 vec3(-50, 100, -50)

#define G vec3(0, -9.8, 0)
#define DT 0.009

void main()
{
    uint gid = gl_GlobalInvocationID.x;

    vec3 p = Positions[gid].xyz;
    vec3 v = Velocities[gid].xyz;

    float gravityBomb1Distance = length(Positions[gid].xyz - gravityBomb1);
    float gravityBomb2Distance = length(Positions[gid].xyz - gravityBomb2);

    float gravityBomb1Acceleration = gravityBomb1Distance / (gravityBomb1Distance + gravityBomb2Distance);
    float gravityBomb2Acceleration = gravityBomb2Distance / (gravityBomb1Distance + gravityBomb2Distance);

    // vec3 va = vec3(v.x * 2.15, v.y, v.z * 2.15);
    // vec3 pp = p + va * DT + 0.5 * DT * DT * G;
    // vec3 vp = v + G * DT;

    vec3 direction
        = mix(normalize(gravityBomb1 - Positions[gid].xyz), normalize(gravityBomb2 - Positions[gid].xyz), gravityBomb2Acceleration);

    vec3 pp = p + v * DT + 0.5 * DT * DT * G;
    vec3 vp = v + direction * DT;

    Positions[gid].xyz = pp;
    Velocities[gid].xyz = vp;
}
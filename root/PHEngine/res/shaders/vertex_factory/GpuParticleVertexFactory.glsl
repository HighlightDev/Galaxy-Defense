#version 440 core

#include "materialCommon.incl.glsl"

in vec4 VertexPosition;

layout(std430, binding = 0) buffer ParticlePositionBuffer
{
    vec4 ParticleRelativePositions[];
};

layout(std430, binding = 3) buffer ParticleColorBuffer
{
    vec4 ParticleColors[];
};

layout(std430, binding = 4) buffer ParticleRotationAndSizeBuffer
{
    vec2 ParticleRotationAndSizes[]; // x - rotation and y - size
};

layout(std140) uniform Matrices
{
    mat4 worldMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

vec4 GetLocalToWorldSpacePosition()
{
    return worldMatrix * ParticleRelativePositions[gl_InstanceID];
}

vec3 GetLocalToWorldSpaceNormal()
{
    return vec3(0);
}

vec3 GetLocalToWorldSpaceTangent()
{
    return vec3(0);
}

vec3 GetLocalToWorldSpaceBitangent()
{
    return vec3(0);
}

vec2 GetLocalTexCoords()
{
    return vec2(0);
}

vec3 GetParticleRelativeOffset()
{
    return ParticleRelativePositions[gl_InstanceID].xyz;
}

vec2 GetParticleRotationAndSize()
{
    return ParticleRotationAndSizes[gl_InstanceID];
}

vec3 GetParticleColor()
{
    return ParticleColors[gl_InstanceID].rgb;
}

MATERIAL_VS_OUTPUT VertexFactoryGetMaterialOutput()
{
    MATERIAL_VS_OUTPUT result;

    result.TextureCoordinates = vec3(vec2(0.0), 0.0);

    vec4 world_pos = GetLocalToWorldSpacePosition();
    vec4 view_pos = viewMatrix * world_pos;
    vec4 projected_pos = projectionMatrix * view_pos;

    result.WorldCoordinates = world_pos;
    result.ViewCoordinates = view_pos;
    result.ProjectedCoordinates = projected_pos;

    result.WorldNormal = GetLocalToWorldSpaceNormal();
    result.WorldTangent = GetLocalToWorldSpaceTangent();
    result.WorldBitangent = GetLocalToWorldSpaceBitangent();

    return result;
}

FLAT_MATERIAL_VS_OUTPUT VertexFactoryGetFlatMaterialOutput()
{
    FLAT_MATERIAL_VS_OUTPUT result;

    result.InstanceID = gl_InstanceID;

    return result;
}
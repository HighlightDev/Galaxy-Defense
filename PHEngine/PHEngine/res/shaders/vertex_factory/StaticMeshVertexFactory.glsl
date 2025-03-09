#version 440

layout(std140) uniform Matrices {
    mat4 worldMatrix1;
    mat4 viewMatrix1;
    mat4 projectionMatrix1;
};

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoords;
in vec3 VertexTangent;
in vec3 VertexBitangent;

#include "materialCommon.incl.glsl"

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec4 GetLocalToWorldSpacePosition()
{
    return worldMatrix * vec4(VertexPosition, 1.0);
}

vec3 GetLocalToWorldSpaceNormal()
{
    return (worldMatrix * vec4(VertexNormal, 0.0)).xyz;
}

vec3 GetLocalToWorldSpaceTangent()
{
    return (worldMatrix * vec4(VertexTangent, 0.0)).xyz;
}

vec3 GetLocalToWorldSpaceBitangent()
{
    return (worldMatrix * vec4(VertexBitangent, 0.0)).xyz;
}

vec2 GetLocalTexCoords()
{
    return VertexTexCoords;
}

MATERIAL_VS_OUTPUT VertexFactoryGetMaterialOutput()
{
    MATERIAL_VS_OUTPUT result;

    result.TextureCoordinates = vec3(VertexTexCoords, 0.0);

    vec4 world_pos = GetLocalToWorldSpacePosition();
    vec4 view_pos = viewMatrix * world_pos;
    vec4 clipped_pos = projectionMatrix * view_pos;

    result.WorldCoordinates = world_pos;
    result.ViewCoordinates = view_pos;
    result.ClippedCoordinates = clipped_pos;

    result.WorldNormal = GetLocalToWorldSpaceNormal();
    result.WorldTangent = GetLocalToWorldSpaceTangent();
    result.WorldBitangent = GetLocalToWorldSpaceBitangent();

    result.InstanceID = float(gl_InstanceID);

    return result;
}
#version 440

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoords;
in vec3 VertexTangent;
in vec3 VertexBitangent;
in vec4 VertexBlendWeights;
in ivec4 VertexBlendIndex;

#include "materialCommon.incl.glsl"

layout(std140) uniform Matrices
{
    mat4 worldMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std140) uniform SkinningMatrices
{
    mat4 bonesMatrices[MaxBones];
};

vec4 ApplySkinningToVec(vec4 vector)
{
    vec4 skinnedVec = vec4(0);
    for (int i = 0; i < MaxWeights; ++i) {
        int blendIndex = VertexBlendIndex[i];

        float blendWeight = VertexBlendWeights[i];
        skinnedVec += ((bonesMatrices[blendIndex] * vector) * blendWeight);
    }

    return skinnedVec;
}

vec4 GetLocalToWorldSpacePosition()
{
    return worldMatrix * ApplySkinningToVec(vec4(VertexPosition, 1.0));
}

vec3 GetLocalToWorldSpaceNormal()
{
    vec4 nSkinnedNormal = normalize(ApplySkinningToVec(vec4(VertexNormal, 0.0)));

    return (worldMatrix * nSkinnedNormal).xyz;
}

vec3 GetLocalToWorldSpaceTangent()
{
    vec4 nSkinnedTangent = normalize(ApplySkinningToVec(vec4(VertexTangent, 0.0)));

    return (worldMatrix * nSkinnedTangent).xyz;
}

vec3 GetLocalToWorldSpaceBitangent()
{
    vec4 nSkinnedBitangent = normalize(ApplySkinningToVec(vec4(VertexBitangent, 0.0)));

    return (worldMatrix * nSkinnedBitangent).xyz;
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
    vec4 projected_pos = projectionMatrix * view_pos;

    result.WorldCoordinates = world_pos;
    result.ViewCoordinates = view_pos;
    result.ProjectedCoordinates = projected_pos;

    result.WorldNormal = GetLocalToWorldSpaceNormal();
    result.WorldTangent = GetLocalToWorldSpaceTangent();
    result.WorldBitangent = GetLocalToWorldSpaceBitangent();

    result.InstanceID = float(gl_InstanceID);

    return result;
}
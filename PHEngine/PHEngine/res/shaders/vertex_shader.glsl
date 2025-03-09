#version 440

layout(location = 0) in vec3 vertex;
layout(location = 2) in vec2 texCoord;

layout(std140) uniform Matrices {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

struct MATERIAL_VS_OUTPUT {
    // For texture 2D only two components are used, for Cube three components are
    // used
    vec3 TextureCoordinates;

    vec4 WorldCoordinates;

    vec4 ViewCoordinates;

    vec4 ClippedCoordinates;

    vec3 WorldNormal;

    vec3 WorldTangent;

    vec3 WorldBitangent;

    float InstanceID;
};

vec3 transformNormalFromTangentSpaceToWorld(in MATERIAL_VS_OUTPUT VsOutput, in vec3 tangentSpaceNormal)
{
    vec3 worldNormal = normalize(VsOutput.WorldNormal);
    vec3 worldTangent = normalize(VsOutput.WorldTangent);
    vec3 worldBitangent = normalize(VsOutput.WorldBitangent);
    mat3 tangentToWorld = mat3(worldTangent, worldBitangent, worldNormal);
    return tangentToWorld * tangentSpaceNormal;
}

vec4 GetLocalToWorldSpacePosition()
{
    return modelMatrix * vec4(vertex, 1.0);
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
    return texCoord;
}

MATERIAL_VS_OUTPUT VertexFactoryGetMaterialOutput()
{
    MATERIAL_VS_OUTPUT result;

    result.TextureCoordinates = vec3(texCoord, 0.0);

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

void main() {
    vec3 worldPosition = vec3(modelMatrix * vec4(vertex, 1.0));

    gl_Position = projectionMatrix * viewMatrix * vec4(worldPosition, 1.0);
}

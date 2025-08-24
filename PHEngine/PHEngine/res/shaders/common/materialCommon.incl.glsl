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

    // float due to perspective interpolation between vertex and fragment shader
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
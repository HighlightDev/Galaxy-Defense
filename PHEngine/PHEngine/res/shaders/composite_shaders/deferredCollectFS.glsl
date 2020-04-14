#version 400

layout (location = 0) out vec4 gBuffer_Position; // xyz: position + w: distance to pixel in view space
layout (location = 1) out vec3 gBuffer_Normal;
layout (location = 2) out vec4 gBuffer_AlbedoNSpecular;

in MATERIAL_VS_OUTPUT VsOutput;

void main()
{
	vec3 albedoColor = GetMaterialAlbedo(VsOutput);
	float metallicFactor = GetMaterialMetallic(VsOutput);

	vec3 normalFromNM = GetMaterialNormalMapNormal(VsOutput);

	vec3 worldNormal = normalize(VsOutput.WorldNormal);
	vec3 worldTangent = normalize(VsOutput.WorldTangent);
	vec3 worldBitangent = normalize(VsOutput.WorldBitangent);

	mat3 tangentToWorld = mat3(worldTangent, worldBitangent, worldNormal);

	normalFromNM = tangentToWorld * normalFromNM;

	gBuffer_Position = vec4(VsOutput.WorldCoordinates, length(VsOutput.ViewCoordinates));

	gBuffer_Normal = normalFromNM;
	gBuffer_AlbedoNSpecular = vec4(albedoColor, 1.0);
}
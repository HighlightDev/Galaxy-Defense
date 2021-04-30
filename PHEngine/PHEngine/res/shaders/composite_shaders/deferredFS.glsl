#version 400

layout (location = 0) out vec3 gBuffer_Position;
layout (location = 1) out vec3 gBuffer_Normal;
layout (location = 2) out vec3 gBuffer_Albedo;
layout (location = 3) out vec2 gBuffer_MetallicRoughness;

in MATERIAL_VS_OUTPUT VsOutput;

void main()
{
	vec3 albedoColor = GetMaterialAlbedo(VsOutput);
	vec2 metallicRoughnessFactor = GetMaterialMetallicRoughness(VsOutput);

	vec3 normalFromNM = GetMaterialNormalMapNormal(VsOutput);
	vec3 worldNormal = normalize(VsOutput.WorldNormal);
	vec3 worldTangent = normalize(VsOutput.WorldTangent);
	vec3 worldBitangent = normalize(VsOutput.WorldBitangent);
	mat3 tangentToWorld = mat3(worldTangent, worldBitangent, worldNormal);
	normalFromNM = tangentToWorld * normalFromNM;

	gBuffer_Position = VsOutput.WorldCoordinates;
	gBuffer_Normal = normalFromNM;
	gBuffer_Albedo = albedoColor;
	gBuffer_MetallicRoughness = metallicRoughnessFactor;
}
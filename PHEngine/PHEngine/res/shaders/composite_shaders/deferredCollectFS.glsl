#version 400

layout (location = 0) out vec3 gBuffer_Position;
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
	
	// TODO: add another gbuffer for distance to pixel
	// move part of code of calculating inside deferred light pass shader
	// bind values to global settings value, size of orthographic projection extent

	float distanceToPixel = length(VsOutput.ViewCoordinates); // distance to pixel, this is needed for shadow transitions
	distanceToPixel = distanceToPixel - (50 - 10);
	distanceToPixel = distanceToPixel / 6;
	distanceToPixel = clamp(1.0, 0.0, distanceToPixel);

	// TODO: SHIIIIT

	gBuffer_Position = VsOutput.WorldCoordinates;

	gBuffer_Normal = normalFromNM;
	gBuffer_AlbedoNSpecular = vec4(albedoColor, 1.0);
}
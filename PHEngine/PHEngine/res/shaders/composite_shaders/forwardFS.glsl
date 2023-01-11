#version 400

layout (location = 0) out vec4 FragColor;

in MATERIAL_VS_OUTPUT VsOutput;

void main()
{
	vec3 albedoColor = GetMaterialAlbedo(VsOutput);
	float alpha = GetMaterialAlphaMask(VsOutput);
	FragColor = vec4(albedoColor, alpha);
}
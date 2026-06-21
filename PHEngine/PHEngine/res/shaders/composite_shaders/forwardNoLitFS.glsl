#version 440 core

layout(location = 0) out vec4 FragColor;

in MATERIAL_VS_OUTPUT VsOutput;
flat in FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

void main()
{
    vec3 albedoColor = GetMaterialAlbedo(VsOutput, FlatVsOutput);
    float alpha = GetMaterialAlphaMask(VsOutput, FlatVsOutput);
    vec4 emissionColor = GetMaterialEmission(VsOutput, FlatVsOutput);
    FragColor = vec4(mix(albedoColor, emissionColor.rgb, emissionColor.a), alpha);
}
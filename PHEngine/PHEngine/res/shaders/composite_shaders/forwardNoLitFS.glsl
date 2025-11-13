#version 440

layout(location = 0) out vec4 FragColor;

in MATERIAL_VS_OUTPUT VsOutput;

void main()
{
    vec3 albedoColor = GetMaterialAlbedo(VsOutput);
    float alpha = GetMaterialAlphaMask(VsOutput);
    vec4 emissionColor = GetMaterialEmission(VsOutput);
    FragColor = vec4(mix(albedoColor, emissionColor.rgb, emissionColor.a), alpha);
}
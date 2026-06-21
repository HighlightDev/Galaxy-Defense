#version 440 core

layout(location = 0) out vec4 FragColor;

in MATERIAL_VS_OUTPUT GsOutput;
flat in FLAT_MATERIAL_VS_OUTPUT FlatGsOutput;

void main()
{
    vec3 color = GetMaterialAlbedo(GsOutput, FlatGsOutput);
    float alpha = GetMaterialAlphaMask(GsOutput, FlatGsOutput);
    FragColor = vec4(color, alpha);
}

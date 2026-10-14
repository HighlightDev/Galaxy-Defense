#version 440 core

layout(location = 0) out vec4 FragColor;

in vec3 out_color;

in MATERIAL_VS_OUTPUT GsOutput;
flat in FLAT_MATERIAL_VS_OUTPUT FlatGsOutput;

void main()
{
    float opacity = GetMaterialAlphaMask(GsOutput, FlatGsOutput);
    FragColor = vec4(out_color, opacity);
}
#version 440 core

layout(location = 0) out vec4 FragColor;

in vec3 out_color;

in MATERIAL_VS_OUTPUT GsOutput;

void main()
{
    float opacity = GetMaterialAlphaMask(GsOutput);
    FragColor = vec4(out_color, opacity);
}
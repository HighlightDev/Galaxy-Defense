#version 440 core

layout(location = 0) out vec4 FragColor;

in MATERIAL_VS_OUTPUT GsOutput;

void main()
{
    vec3 color = GetMaterialAlbedo(GsOutput);
    float alpha = GetMaterialAlphaMask(GsOutput);
    FragColor = vec4(color, alpha);
}

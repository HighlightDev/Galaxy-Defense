#version 440 core

out MATERIAL_VS_OUTPUT VsOutput;

void main()
{
    VsOutput = VertexFactoryGetMaterialOutput();
    gl_Position = VsOutput.ViewCoordinates;
}

#version 440 core

out MATERIAL_VS_OUTPUT VsOutput;
out FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

void main()
{
    VsOutput = VertexFactoryGetMaterialOutput();
    FlatVsOutput = VertexFactoryGetFlatMaterialOutput();

    gl_Position = VsOutput.ViewCoordinates;
}

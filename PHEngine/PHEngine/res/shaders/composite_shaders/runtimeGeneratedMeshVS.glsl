#version 440 core

#include "materialCommon.incl.glsl"

out MATERIAL_VS_OUTPUT VsOutput;
flat out FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

void main(void)
{
    VsOutput = VertexFactoryGetMaterialOutput();
    FlatVsOutput = VertexFactoryGetFlatMaterialOutput();

    gl_Position = VsOutput.ProjectedCoordinates;
}
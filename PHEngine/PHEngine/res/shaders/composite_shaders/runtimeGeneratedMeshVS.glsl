#version 440 core

#include "materialCommon.incl.glsl"

out MATERIAL_VS_OUTPUT VsOutput;

void main(void)
{
    VsOutput = VertexFactoryGetMaterialOutput();

    gl_Position = VsOutput.ProjectedCoordinates;
}
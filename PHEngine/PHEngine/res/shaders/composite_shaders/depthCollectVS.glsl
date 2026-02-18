#version 440 core

#include "materialCommon.incl.glsl"

out MATERIAL_VS_OUTPUT VsOutput;

void main()
{

    VsOutput = VertexFactoryGetMaterialOutput();

    gl_Position = VsOutput.ProjectedCoordinates;
}
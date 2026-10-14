#version 440 core

#include "materialCommon.incl.glsl"

out MATERIAL_VS_OUTPUT VsOutput;
flat out FLAT_MATERIAL_VS_OUTPUT FlatVsOutput;

uniform vec4 clipPlane;

void main(void)
{
    VsOutput = VertexFactoryGetMaterialOutput();
    FlatVsOutput = VertexFactoryGetFlatMaterialOutput();

    gl_ClipDistance[0] = dot(vec4(clipPlane.xyz, -clipPlane.w), vec4(VsOutput.WorldCoordinates.xyz, 1.0));
    gl_Position = VsOutput.ProjectedCoordinates;
}
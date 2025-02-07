#version 400

#include "materialCommon.incl.glsl"

out MATERIAL_VS_OUTPUT VsOutput;

uniform vec4 clipPlane;

void main(void)
{
    VsOutput = VertexFactoryGetMaterialOutput();

    gl_ClipDistance[0] = dot(vec4(clipPlane.xyz, -clipPlane.w), vec4(VsOutput.WorldCoordinates.xyz, 1.0));
    gl_Position = VsOutput.ClippedCoordinates;
}
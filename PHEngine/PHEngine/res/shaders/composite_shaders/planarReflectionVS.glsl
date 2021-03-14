#version 400

#include "materialCommon.incl"

out MATERIAL_VS_OUTPUT VsOutput;

uniform vec4 clipPlane;

void main(void)
{
	VsOutput = VertexFactoryGetMaterialOutput();

	gl_ClipDistance[0] = dot(clipPlane, vec4(VsOutput.WorldCoordinates, 1.0));
	gl_Position = VsOutput.ClippedCoordinates;
}
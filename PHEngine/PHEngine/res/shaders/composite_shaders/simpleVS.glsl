#version 400

#include "materialCommon.incl"

out MATERIAL_VS_OUTPUT VsOutput;

void main(void)
{
	VsOutput = VertexFactoryGetMaterialOutput();

	gl_Position = VsOutput.ClippedCoordinates;
}
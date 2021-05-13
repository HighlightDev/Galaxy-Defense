#version 400

#include "materialCommon.incl"

out MATERIAL_VS_OUTPUT VsOutput;

void main(){

	VsOutput = VertexFactoryGetMaterialOutput();

	gl_Position = VsOutput.ClippedCoordinates;
}
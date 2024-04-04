#version 400

#include "materialCommon.incl.glsl"

in MATERIAL_VS_OUTPUT VsOutput;

uniform bool bWriteDepthLinearly;
uniform vec3 lightWorldPosition;
uniform float shadowDistance;

float GetLinearDepth()
{
	float distanceToLight = length(VsOutput.WorldCoordinates.xyz - lightWorldPosition);
	distanceToLight /= shadowDistance; // map to [0;1] range
	return distanceToLight;
}

void main()
{
	if (bWriteDepthLinearly)
	{
		gl_FragDepth = GetLinearDepth();
	}
	else
	{
		gl_FragDepth = gl_FragCoord.z;
	}
}


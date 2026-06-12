#version 440 core

#include "materialCommon.incl.glsl"

in MATERIAL_VS_OUTPUT VsOutput;

uniform bool bWriteDepthLinearly;
uniform vec3 lightWorldPosition;
uniform float invShadowDistance;

float GetLinearDepth() {
    float distanceToLight = length(VsOutput.WorldCoordinates.xyz - lightWorldPosition);
    distanceToLight *= invShadowDistance; // map to [0;1] range
    return distanceToLight;
}

void main() {
    gl_FragDepth = bWriteDepthLinearly ? GetLinearDepth() : gl_FragCoord.z;
}

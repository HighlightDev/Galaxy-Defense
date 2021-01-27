#version 400

in vec4 worldPosition;

uniform vec3 spotlightPos;
uniform float farPlane;

float GetLinearDepth()
{
	float distanceToLight = length(worldPosition.xyz - spotlightPos);
	distanceToLight /= farPlane; // map to [0;1] range
	return distanceToLight;
}

void main(void)
{
	gl_FragDepth = GetLinearDepth();
}

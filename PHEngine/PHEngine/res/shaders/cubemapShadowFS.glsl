#version 400

in vec3 FragPos;

uniform vec3 pointLightPos;
uniform float farPlane;

float GetLinearDepth()
{
	float distanceToLight = length(FragPos - pointLightPos);
	distanceToLight /= farPlane; // map to [0;1] range
	return distanceToLight;
}

void main(void)
{
	gl_FragDepth = GetLinearDepth();
}
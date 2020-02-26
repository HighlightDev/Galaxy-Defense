#version 400

#define billboardSize 1
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2[] billboardScale;

smooth out vec2 texCoords;

uniform mat4 projectionMatrix;

void buildBillboard(in float width, in float height)
{
	gl_Position = projectionMatrix * (vec4(-width, -height, 0.0, 0.0) + gl_in[0].gl_Position);
	texCoords = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = projectionMatrix * (vec4(width, -height, 0.0, 0.0) + gl_in[0].gl_Position);
	texCoords = vec2(1.0, 1.0);
	EmitVertex();

	gl_Position = projectionMatrix * (vec4(-width, height, 0.0, 0.0) + gl_in[0].gl_Position);
	texCoords = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = projectionMatrix * (vec4(width, height, 0.0, 0.0) + gl_in[0].gl_Position);
	texCoords = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

void main(void)
{
    float billboardWidth = billboardSize * billboardScale[0].x;
	float billboardHeight = billboardSize * billboardScale[0].y;
	buildBillboard(billboardWidth, billboardHeight);
}
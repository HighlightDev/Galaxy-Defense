#version 440

#include "materialCommon.incl.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float extent;
uniform vec2 screenResolution;
uniform bool applyScreenAspectRatio;
uniform float rotationRadians;

in MATERIAL_VS_OUTPUT VsOutput[];

out MATERIAL_VS_OUTPUT GsOutput;

mat4 rollMatrix(float radAngle)
{
    float s = sin(radAngle);
    float c = cos(radAngle);

    return mat4(c, -s, 0.0, 0.0, s, c, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

void main()
{
    mat4 rotMatrix = rollMatrix(rotationRadians);

    float aspectRatio = screenResolution.x / screenResolution.y;
    vec4 vertex1 = vec4(-extent, extent * aspectRatio, 0.0, 0.0);
    vec4 vertex2 = vec4(-extent, -extent * aspectRatio, 0.0, 0.0);
    vec4 vertex3 = vec4(extent, extent * aspectRatio, 0.0, 0.0);
    vec4 vertex4 = vec4(extent, -extent * aspectRatio, 0.0, 0.0);

    vertex1 = rotMatrix * vertex1;
    vertex2 = rotMatrix * vertex2;
    vertex3 = rotMatrix * vertex3;
    vertex4 = rotMatrix * vertex4;

    vec3 texCoordsVertex1 = vec3(0.0, 1.0, 0.0);
    vec3 texCoordsVertex2 = vec3(0.0, 0.0, 0.0);
    vec3 texCoordsVertex3 = vec3(1.0, 1.0, 0.0);
    vec3 texCoordsVertex4 = vec3(1.0, 0.0, 0.0);

    gl_Position = projectionMatrix * (vertex1 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex1;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex2 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex2;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex3 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex3;
    EmitVertex();

    gl_Position = projectionMatrix * (vertex4 + gl_in[0].gl_Position);
    GsOutput = VsOutput[0];
    GsOutput.TextureCoordinates = texCoordsVertex4;
    EmitVertex();

    EndPrimitive();
}

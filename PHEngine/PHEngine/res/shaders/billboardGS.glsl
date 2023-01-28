#version 400

#include "materialCommon.incl"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projectionMatrix;
uniform float extent;

in MATERIAL_VS_OUTPUT VsOutput[];

out MATERIAL_VS_OUTPUT GsOutput;

void main() {
  vec4 vertex1 = vec4(-extent, extent, 0.0, 1.0);
  vec4 vertex2 = vec4(-extent, -extent, 0.0, 1.0);
  vec4 vertex3 = vec4(extent, extent, 0.0, 1.0);
  vec4 vertex4 = vec4(extent, -extent, 0.0, 1.0);

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

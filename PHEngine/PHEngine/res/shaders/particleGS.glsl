#version 400

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projectionMatrix;
uniform vec4 color;

in vec2 vs_out_rotation_size[];
in vec4 vs_out_particle_color[];

out vec4 out_color;

mat4 rollMatrix(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    
    return mat4(c, -s, 0.0, 0.0,
                s, c, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

void main()
{
    vec2 rotation_size = vs_out_rotation_size[0];

    out_color = vs_out_particle_color[0];
    
    mat4 rotationMatrix = rollMatrix(rotation_size.x);

    vec4 rotatedVertex1 = rotationMatrix * vec4(-rotation_size.y, rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex2 = rotationMatrix * vec4(-rotation_size.y, -rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex3 = rotationMatrix * vec4(rotation_size.y, rotation_size.y, 0.0, 1.0);
    vec4 rotatedVertex4 = rotationMatrix * vec4(rotation_size.y, -rotation_size.y, 0.0, 1.0);
    
    gl_Position = projectionMatrix * (rotatedVertex1 + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex2 + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex3 + gl_in[0].gl_Position);
    EmitVertex();

    gl_Position = projectionMatrix * (rotatedVertex4 + gl_in[0].gl_Position);
    EmitVertex();

    EndPrimitive();
}
#version 400

layout (location = 0) out vec4 FragColor;

in vec4 out_color;

void main()
{
    FragColor = out_color;
}
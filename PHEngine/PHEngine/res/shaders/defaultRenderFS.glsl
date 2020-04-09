#version 330

layout (location = 0) out vec4 FragColor;

const vec3 COLOR = vec3(0.8, 0.8, 0.8);

void main(void)
{
	FragColor = vec4(COLOR, 0.1);
}
#version 440 core

layout(location = 0) out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D image;
uniform float isCustomColor;
uniform vec3 color;
uniform float opacity;

void main(void)
{
    vec4 texSampleColor = texture(image, texCoords);
    vec3 resultColor = (isCustomColor * color) + ((1.0 - isCustomColor) * texSampleColor.rgb);
    FragColor = opacity * vec4(resultColor, texSampleColor.a);
}
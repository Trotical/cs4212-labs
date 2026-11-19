#version 330 core

layout(location = 0) out vec4 fragmentColor;

in vec4 normal;

void main(void)
{
    vec3 n = normalize(normal.xyz);
    vec3 mappedColor = 0.5 * (n + vec3(1.0));
    fragmentColor = vec4(mappedColor, 1.0);
}
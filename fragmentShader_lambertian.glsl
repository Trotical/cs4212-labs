#version 330 core

layout(location = 0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec3 lightIntensity; // optional: defaults to (1,1,1) if not set

in vec4 normal;
in vec4 lightDir;

void main(void)
{
    vec3 N = normalize(normal.xyz);
    vec3 L = normalize(lightDir.xyz);

    float val = max(0.0, dot(N, L));
    vec3 intensity = (length(lightIntensity) > 0.0) ? lightIntensity : vec3(1.0);
    vec3 diffuseShading = diffuseComponent * intensity * val;

    fragmentColor = vec4(diffuseShading, 1.0);
}
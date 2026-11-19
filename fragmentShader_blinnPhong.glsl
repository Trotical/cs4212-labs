#version 330 core

layout(location = 0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExp;
uniform vec3 camPosWorld;
uniform vec3 lightIntensity;

in vec4 normal;
in vec4 lightDir;
in vec4 fragWorldPos;

void main(void)
{
    vec3 N = normalize(normal.xyz);
    vec3 L = normalize(lightDir.xyz);
    vec3 V = normalize(camPosWorld - fragWorldPos.xyz);
    vec3 H = normalize(L + V);

    float diff = max(0.0, dot(N, L));
    float spec = (diff > 0.0) ? pow(max(0.0, dot(N, H)), phongExp) : 0.0;

    vec3 I = (length(lightIntensity) > 0.0) ? lightIntensity : vec3(1.0);
    vec3 colorOut = diffuseComponent * I * diff + specularComponent * I * spec;

    fragmentColor = vec4(colorOut, 1.0);
}
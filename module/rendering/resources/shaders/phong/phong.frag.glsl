#version 450
#include "../utils.glsl"
#include "../common.glsl"

layout(location = 1) in vec3 normal;
layout(location = 0) out vec4 outColor;

vec3 diffuse(vec3 lightDir, vec3 normalDir, Lighting light) {
    float diff = max(dot(normalDir, lightDir), 0.0F);
    return (light.color.xyz * diff * light.intensity);
}

void main() {
    vec3 lightDir = vec3(-1, -1, 1);
    float intensity = dot(lightDir, normal);
    Lighting lighting;
    lighting.intensity = 1;
    lighting.color = vec3(0, 1, 1);
    outColor = vec4(diffuse(lightDir, normal, lighting), 1.0);
    //outColor = vec4(normalToColor(normal), 1.0);
}


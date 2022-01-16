#version 450
#include "utils.glsl"
#include "common.glsl"
#include "phong.frag.gen.glsl"

vec3 diffuse(vec3 lightDir, vec3 normalDir, Lighting light) {
    float diff = max(dot(normalDir, lightDir), 0.0F);
    return (light.color.xyz * diff * light.intensity);
}

void main() {
    /*vec3 lightDir = vec3(-1, -1, 1);
    float intensity = dot(lightDir, normal);
    Lighting lighting;
    lighting.intensity = 1;
    lighting.color = vec3(0, 1, 1);
    vec3 diff = diffuse(lightDir, normal, lighting);*/
//    gl_Color = vec4(diff + spec, 1.0);
}


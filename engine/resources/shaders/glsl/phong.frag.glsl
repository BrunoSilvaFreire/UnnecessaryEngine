#version 450
#include "utils.glsl"
layout(location = 0) in vec3 normal;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(normalToColor(normal), 1.0);
}


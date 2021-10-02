#version 450 core

#include "common.glsl"

layout(set = 1, binding = 0) uniform cameraData
{
    Matrices matrices;
} cam;

layout(push_constant) uniform pushConstants
{
    PerObjectData data;
} pc;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inNormal;
layout(location = 0) out vec3 normal;

void main()
{
    mat4 mvp = cam.matrices.vp * pc.data.model;
    gl_Position = mvp * vec4(position, 1.0);
    normal = inNormal;
}


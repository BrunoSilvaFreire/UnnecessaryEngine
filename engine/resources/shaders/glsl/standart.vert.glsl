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
layout(location = 0) out vec4 outPos;

void main()
{
    mat4 mvp = cam.matrices.vp * pc.data.model;
    outPos = mvp * vec4(position, 1.0);
}


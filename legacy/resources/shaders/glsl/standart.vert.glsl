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
out gl_PerVertex{
    vec4 gl_Position;
};
void main()
{
    mat4 mvp = cam.matrices.vp * pc.data.model;
    gl_Position = mvp * vec4(position, 1.0);
}


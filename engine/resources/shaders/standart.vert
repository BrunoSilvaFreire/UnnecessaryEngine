#version 450
#include "common.glsl"

layout(push_constant) uniform constants
{
    PerObjectData data;
} PushConstants;

layout(set = 0) Matrices matrices;
layout(location = 0) in vec3 positionIn;
layout(location = 0) out vec3 positionOut;

void main() {
    gl_Position = (matrices.vp * PushConstants.data.model) * vec4(positionIn, 1);
}
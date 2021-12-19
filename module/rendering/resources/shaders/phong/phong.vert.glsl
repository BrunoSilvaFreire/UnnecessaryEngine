#version 450 core

#include "../common.glsl"
#include "phong.vert.gen.glsl"


void main() {
    mat4 mvp = cam.matrices.vp * pc.data.model;
    gl_Position = mvp * vec4(position, 1.0);
    normal = inNormal;
}


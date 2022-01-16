#version 450 core

#include "common.glsl"
#include "phong.vert.gen.glsl"


void main() {
    mat4 mvp = u_vp_matrix.vp_matrix * u_model_matrix.model_matrix;
    gl_Position = mvp * vec4(vertex_position, 1.0);
}


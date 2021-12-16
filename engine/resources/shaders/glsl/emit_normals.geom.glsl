#version 450 core
layout(triangles) in;
layout(max_vertices = 3, triangle_strip) out;

struct GeometryInput
{
    vec3 position;
};

layout(location = 1) out vec3 outNormal;

void main()
{

    vec3 a = gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[1].gl_Position.xyz;
    vec3 c = gl_in[2].gl_Position.xyz;
    vec3 normal = normalize(cross(c - a, b - a));
    for (int i = 0; i < 3; i++)
    {
        outNormal = normal;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}


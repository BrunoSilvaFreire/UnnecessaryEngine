#version 450 core
layout(triangles) in;
layout(max_vertices = 3, triangle_strip) out;

struct GeometryInput
{
    vec3 position;
};

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 0) in vec3 inPos[3];

void main()
{

    vec3 normal = normalize(cross(inPos[2] - inPos[0], inPos[1] - inPos[0]));
    for (int i = 0; i < 3; i++)
    {
        outPos = vec4(inPos[i], 1.0);
        outNormal = normal;
        EmitVertex();
    }
    EndPrimitive();
}


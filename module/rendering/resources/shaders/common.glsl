
struct PerObjectData
{
    mat4 model;
};

struct Matrices
{
    mat4 vp;
};

struct Lighting
{
    vec3 color;
    float intensity;
};

struct PointLight
{
    Lighting lighting;
    vec3 position;
};

struct PhongData {
    vec3 viewDir;
};
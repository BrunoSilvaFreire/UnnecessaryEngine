struct PerObjectData {
    mat4 model;
};

struct Matrices {
    mat4 vp;
};

struct Lighting {
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    Lighting lighting;
};
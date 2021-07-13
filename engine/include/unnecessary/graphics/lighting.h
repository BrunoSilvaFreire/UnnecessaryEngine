#ifndef UNNECESSARYENGINE_LIGHTING_H
#define UNNECESSARYENGINE_LIGHTING_H

#include <glm/glm.hpp>

namespace un {
    union Lighting {
        struct {
            float r, g, b, w;
        };
        struct {
            glm::vec3 color;
            float intensity;
        };
    };

    struct PointLight {
        Lighting lighting;
    };

    struct PointLightData {
        glm::vec3 position;
        PointLight light;
    };
#ifndef MAX_NUM_LIGHTS
#define MAX_NUM_LIGHTS 4
#endif
    struct ObjectLightingData {
        u32 nPointLights;
        u32 pointLightsIndices[MAX_NUM_LIGHTS];
    };
    struct SceneLightingData {
        u32 numPointLights;
        PointLightData* pointLights;
    };
}
#endif

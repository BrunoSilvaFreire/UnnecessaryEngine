#ifndef UNNECESSARYENGINE_MATERIAL_H
#define UNNECESSARYENGINE_MATERIAL_H

#include <unnecessary/algorithm/layout.h>
#include <unnecessary/graphics/shading/shader.h>

#include <unordered_map>
#include <utility>

namespace un {
    template<typename T>
    class MaterialProperties {
    private:
        std::unordered_map<std::string, T> properties;
    public:
        T operator[](std::string key) {
            return properties[key];
        }
    };

    class Material {
    private:
        un::Pipeline *shader;
        MaterialProperties<float> floats;
        MaterialProperties<int> ints;
    public:
        explicit Material(un::Pipeline *shader);

        un::Pipeline *getShader() const;
    };
}
#endif

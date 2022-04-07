#ifndef UNNECESSARYENGINE_GLSL_TYPE_H
#define UNNECESSARYENGINE_GLSL_TYPE_H

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.hpp>

namespace un {
    class GlslType {
    private:
        std::size_t singleSize;
        std::size_t numElements;
        vk::Format format;
    public:
        GlslType();

        GlslType(size_t singleSize, size_t numElements, vk::Format format);

        size_t getSingleSize() const;

        size_t getNumElements() const;

        vk::Format getFormat() const;
    };


    class GlslTypeDatabase {
    private:
        std::unordered_map<std::string, un::GlslType> types;
        std::unordered_map<std::string, std::string> glsl2Cpp;

        void translate(std::string glsl, std::string cpp);

    public:
        GlslTypeDatabase();

        bool tryFind(std::string name, un::GlslType** pType);

        std::string glslToCpp(const std::string& basicString);
    };
}
#endif

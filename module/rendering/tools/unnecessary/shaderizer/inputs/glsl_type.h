#ifndef UNNECESSARYENGINE_GLSL_TYPE_H
#define UNNECESSARYENGINE_GLSL_TYPE_H

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.hpp>

namespace un {
    class glsl_type {
    private:
        std::size_t _singleSize;
        std::size_t _numElements;
        vk::Format _format;

    public:
        glsl_type();

        glsl_type(size_t singleSize, size_t numElements, vk::Format format);

        size_t get_single_size() const;

        size_t get_num_elements() const;

        vk::Format get_format() const;
    };

    class glsl_type_database {
    private:
        std::unordered_map<std::string, glsl_type> _types;
        std::unordered_map<std::string, std::string> _glsl2Cpp;

        void translate(std::string glsl, std::string cpp);

    public:
        glsl_type_database();

        bool try_find(std::string name, glsl_type** pType);

        std::string glsl_to_cpp(const std::string& basicString);
    };
}
#endif

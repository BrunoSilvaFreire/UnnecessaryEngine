#include <unnecessary/shaderizer/inputs/glsl_type.h>

namespace un {
    glsl_type::glsl_type() {
    }

    glsl_type::glsl_type(
        size_t singleSize,
        size_t numElements,
        vk::Format format
    ) : singleSize(
        singleSize
    ), numElements(numElements), format(format) {
    }

    size_t glsl_type::get_single_size() const {
        return singleSize;
    }

    size_t glsl_type::get_num_elements() const {
        return numElements;
    }

    vk::Format glsl_type::get_format() const {
        return format;
    }

    bool glsl_type_database::try_find(std::string name, GlslType** pType) {
        const auto& iterator = types.find(name);
        if (iterator != types.end()) {
            *pType = &iterator->second;
            return true;
        }
        pType = nullptr;
        return false;
    }

    glsl_type_database::glsl_type_database() {
        // Generate vec's
        std::array<vk::Format, 3> vecFormat = {
            vk::Format::eR32G32Sfloat,
            vk::Format::eR32G32B32Sfloat,
            vk::Format::eR32G32B32A32Sfloat
        };
        std::array<vk::Format, 3> matForms = {
            vk::Format::eUndefined,
            vk::Format::eUndefined,
            vk::Format::eUndefined,
        };
        for (std::size_t i = 2; i <= 4; ++i) {
            std::string name = "vec";
            name += std::to_string(i);
            std::string cpp = "glm::vec";
            translate(name, cpp + std::to_string(i));
            types.emplace(
                name,
                glsl_type(
                    sizeof(float),
                    i,
                    vecFormat[i - 2]
                )
            );
        }
        for (std::size_t i = 2; i <= 4; ++i) {
            std::string name = "mat";
            name += std::to_string(i);
            std::string cpp = "glm::mat";
            translate(name, cpp + std::to_string(i));
            types[name] = glsl_type(
                sizeof(float),
                i * i,
                matForms[i - 2]
            );
        }
    }

    void glsl_type_database::translate(std::string glsl, std::string cpp) {
        glsl2Cpp[glsl] = cpp;
    }

    std::string glsl_type_database::glsl_to_cpp(const std::string& basicString) {
        return glsl2Cpp[basicString];
    }
}


#include <unnecessary/shaderizer/inputs/glsl_type.h>

namespace un {
    GlslType::GlslType() {}

    GlslType::GlslType(
        size_t singleSize,
        size_t numElements,
        vk::Format format
    ) : singleSize(
        singleSize
    ), numElements(numElements), format(format) {}

    size_t GlslType::getSingleSize() const {
        return singleSize;
    }

    size_t GlslType::getNumElements() const {
        return numElements;
    }

    vk::Format GlslType::getFormat() const {
        return format;
    }

    bool GlslTypeDatabase::tryFind(std::string name, un::GlslType** pType) {
        const auto& iterator = types.find(name);
        if (iterator != types.end()) {
            *pType = &iterator->second;
            return true;
        } else {
            pType = nullptr;
            return false;
        }
    }

    GlslTypeDatabase::GlslTypeDatabase() {
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
        };;
        for (std::size_t i = 2; i <= 4; ++i) {
            std::string name = "vec";
            name += std::to_string(i);
            std::string cpp = "glm::vec";
            translate(name, cpp + std::to_string(i));
            types.emplace(
                name,
                un::GlslType(
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
            types[name] = un::GlslType(
                sizeof(float),
                i * i,
                matForms[i - 2]
            );
        }
    }

    void GlslTypeDatabase::translate(std::string glsl, std::string cpp) {
        glsl2Cpp[glsl] = cpp;
    }

    std::string GlslTypeDatabase::glslToCpp(const std::string& basicString) {
        return glsl2Cpp[basicString];
    }
}
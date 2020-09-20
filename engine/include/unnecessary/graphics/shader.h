#ifndef UNNECESSARYENGINE_SHADER_H
#define UNNECESSARYENGINE_SHADER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/shader_stage.h>
#include <boost/graph/adjacency_matrix.hpp>

namespace un {
    typedef boost::adjacency_matrix<boost::directedS, un::ShaderStage> ShaderModuleGraph;

    class Shader {
    private:
        ShaderModuleGraph moduleGraph;
        vk::PipelineLayout layout;
        vk::Pipeline pipeline;
    public:
        Shader(
                vk::Device &device,
                std::initializer_list<const ShaderStage> shaders
        );
    };
}
#endif

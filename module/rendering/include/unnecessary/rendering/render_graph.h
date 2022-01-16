#ifndef UNNECESSARYENGINE_RENDER_GRAPH_H
#define UNNECESSARYENGINE_RENDER_GRAPH_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphs/dependency_graph.h>

namespace un {
    class RenderPass;

    class RenderGraph : public un::DependencyGraph<un::RenderPass*> {
    private:
        std::optional<vk::RenderPass> renderPass;
    public:
        template<typename T, typename ...Args>
        void enqueuePass(Args... args) {
            add(new T(args...));
        }

        void bake(vk::Device device);

        vk::RenderPass getVulkanPass() const {
            return *renderPass;
        }
    };
};
#endif //UNNECESSARYENGINE_RENDER_GRAPH_H

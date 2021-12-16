
#ifndef UNNECESSARYENGINE_RENDER_GROUP_H
#define UNNECESSARYENGINE_RENDER_GROUP_H

#include <entt/entt.hpp>
#include <vulkan/vulkan.hpp>

namespace un {
    class RenderGroup {
    protected:
        std::vector<entt::entity> entitiesToRender;
    public:
        virtual void configure(vk::CommandBuffer& buffer) = 0;
    };

    class IndexedRenderGroup : public RenderGroup {
    public:
        void configure(vk::CommandBuffer& buffer) override {
        }
    };
}
#endif //UNNECESSARYENGINE_RENDER_GROUP_H

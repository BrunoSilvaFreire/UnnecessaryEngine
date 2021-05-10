#ifndef UNNECESSARYENGINE_DRAWING_H
#define UNNECESSARYENGINE_DRAWING_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/renderer.h>

namespace un {
    class DrawingSystem : public System {
    private:
        un::Renderer * renderer;
        glm::vec4 clearColor;
        vk::RenderPass renderPass;
        vk::SubpassContents contents;
        vk::Rect2D renderArea;
        un::CommandBuffer buffer;
    public:
        DrawingSystem(Renderer &renderer);

        void step(World &world, f32 delta) override;
    };
}

#endif //UNNECESSARYENGINE_DRAWING_H

#ifndef UNNECESSARYENGINE_DRAWING_H
#define UNNECESSARYENGINE_DRAWING_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/jobs/jobs.h>

namespace un {
    class DrawingSystem : public System {
    private:
        un::Renderer * renderer;
        glm::vec4 clearColor;
        vk::RenderPass renderPass;
        u8 currentFramebufferIndex;
        std::vector<vk::Framebuffer> framebuffers;
        vk::Semaphore imageAvailableSemaphore;
        un::ResizableBuffer lightingBuffer;
        vk::DescriptorSet lightingSet;
    public:
        DrawingSystem(Renderer &renderer);

        void step(World &world, f32 delta, un::JobWorker *worker) override;

        const vk::RenderPass &getRenderPass() const;

        vk::Framebuffer nextFramebuffer(u32 *framebufferIndexResult);
    };
}

#endif //UNNECESSARYENGINE_DRAWING_H

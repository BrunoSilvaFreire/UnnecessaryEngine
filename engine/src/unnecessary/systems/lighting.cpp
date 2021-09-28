#include <unnecessary/systems/lighting.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/buffers/buffer_writer.h>
#include <unnecessary/misc/binary_writer.h>
#include "unnecessary/graphics/descriptors/descriptor_writer.h"

namespace un {
    LightingSystem::LightingSystem(
        u32 maxNumLightsPerObject,
        un::Renderer* device
    ) : maxNumLightsPerObject(maxNumLightsPerObject),
        renderer(device),
        lightsBuffer(
            *device,
            vk::BufferUsageFlagBits::eUniformBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
            sizeof(un::SceneLightingData),
            false,
            vk::MemoryPropertyFlagBits::eHostVisible
        ) {
        lightsDirty = true;
        un::DescriptorSetLayout sceneDescriptorLayout;
        sceneDescriptorLayout.push<un::ObjectLightingData>(
            "sceneLighting",
            vk::DescriptorType::eUniformBuffer
        );
        lightsSetLayout = new un::DescriptorAllocator(
            std::move(sceneDescriptorLayout),
            device->getVirtualDevice(),
            1,
            vk::ShaderStageFlagBits::eAllGraphics
        );
        lightsDescriptorSet = lightsSetLayout->allocate();
    }

    void LightingSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        auto& registry = world.getRegistry();
        auto allPointLights = registry.view<un::PointLight>();
        size_t numLights = allPointLights.size();
        runtimeScenePointLights.resize(numLights);
        size_t index = 0;
        for (entt::entity entity : allPointLights) {
            auto& light = runtimeScenePointLights[index++];
            un::Translation* position = registry.try_get<un::Translation>(entity);
            if (position != nullptr) {
                light.position = position->value;
            }
            light.light = registry.get<un::PointLight>(entity);
        }
        if (lightsDirty) {
            //size_t bufSize = 32 * (8 * sizeof(float)) + sizeof(u32);
            size_t bufSize = 1040;
            un::BinaryWriter buf(bufSize);
            buf.write<u32>(numLights);
            /*const size_t ssboHeader = 16;
            buf.skip(ssboHeader - sizeof(u32));*/
            for (const auto& item : runtimeScenePointLights) {
                buf.write(item);
            }
            lightsBuffer.ensureLargeEnough(*renderer, bufSize);
            {
                un::BufferWriter writer(
                    renderer,
                    worker->getGraphicsResources().getCommandPool(),
                    false
                );
                un::DescriptorWriter descriptorWriter(renderer);
                descriptorWriter.updateUniformDescriptor(
                    lightsDescriptorSet,
                    0,
                    lightsBuffer,
                    vk::DescriptorType::eUniformBuffer
                );
                writer.overwrite(lightsBuffer, buf.getBuffer());
                updateGPULightingDataCommandIndex = frameGraphSystem->enqueuePreparationPhase(
                    *writer.getCommandBuffer()
                );
            }
            lightsDirty = false;
        }
        un::SceneLightingData sceneLightingData{};
        sceneLightingData.numPointLights = runtimeScenePointLights.size();
        sceneLightingData.pointLights = runtimeScenePointLights.data();
        un::BufferWriter objectBufferWriter(renderer, vk::CommandPool(), false);
        std::vector<float> distances;
        distances.resize(maxNumLightsPerObject);
    }

    size_t LightingSystem::findSceneBufferSize() {
        size_t size = 0;
        size += sizeof(u32); //nLights
        size += 12; //Padding
        size += runtimeScenePointLights.size() * sizeof(un::PointLightData);
        return size;
    }

    u32 LightingSystem::getMaxNumLightsPerObject() const {
        return maxNumLightsPerObject;
    }

    const std::vector<un::PointLightData>&
    LightingSystem::getRuntimeScenePointLights() const {
        return runtimeScenePointLights;
    }

    const ResizableBuffer& LightingSystem::getSceneLightingBuffer() const {
        return lightsBuffer;
    }

    void LightingSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kUploadFrameData);
        frameGraphSystem = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
    }

    u32 LightingSystem::getUpdateGpuLightingDataCommandIndex() const {
        return updateGPULightingDataCommandIndex;
    }

    const vk::DescriptorSet& LightingSystem::getLightsDescriptorSet() const {
        return lightsDescriptorSet;
    }

    DescriptorAllocator* LightingSystem::getLightsSetLayout() const {
        return lightsSetLayout;
    }

}
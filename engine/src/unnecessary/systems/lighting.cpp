#include <unnecessary/systems/lighting.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/buffers/buffer_writer.h>
#include <unnecessary/misc/binary_writer.h>

namespace un {
    LightingSystem::LightingSystem(
        u32 maxNumLightsPerObject,
        un::Renderer* device
    ) : maxNumLightsPerObject(maxNumLightsPerObject),
        renderer(device),
        sceneLightingBuffer(
            *device,
            vk::BufferUsageFlagBits::eStorageBuffer |
            vk::BufferUsageFlagBits::eTransferDst,
            sizeof(un::SceneLightingData),
            false,
            vk::MemoryPropertyFlagBits::eHostCoherent |
            vk::MemoryPropertyFlagBits::eHostVisible
        ),
        lightsRewritten() {
        lightsDirty = true;
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
        lightsRewritten.set(lightsDirty);
        if (lightsDirty) {
            size_t bufSize = findSceneBufferSize();
            un::BinaryWriter buf(bufSize);
            buf.write<u32>(numLights);
            const size_t ssboHeader = 16;
            buf.skip(ssboHeader - sizeof(u32));
            for (const auto& item : runtimeScenePointLights) {
                buf.write(item);
            }
            sceneLightingBuffer.ensureLargeEnough(*renderer, bufSize);
            {
                un::BufferWriter writer(
                    renderer,
                    worker->getGraphicsResources().getCommandPool(),
                    false
                );
                writer.overwrite(sceneLightingBuffer, buf.getBuffer());
                updateGPULightingDataCommandIndex = frameGraphSystem->enqueuePreparationPhase(
                    *writer.getCommandBuffer(),
                    vk::PipelineStageFlagBits::eTopOfPipe
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
        return sceneLightingBuffer;
    }

    const BooleanHistoric& LightingSystem::getLightsRewritten() const {
        return lightsRewritten;
    }

    void LightingSystem::describe(SystemDescriptor& descriptor) {
        ExplicitSystem::describe(descriptor);
        frameGraphSystem = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
    }

    u32 LightingSystem::getUpdateGpuLightingDataCommandIndex() const {
        return updateGPULightingDataCommandIndex;
    }

}
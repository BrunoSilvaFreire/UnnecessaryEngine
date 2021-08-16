//
// Created by bruno on 12/07/2021.
//

#include <unnecessary/systems/lighting.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/buffers/buffer_writer.h>

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
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
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
            u8* buf = new u8[bufSize];
            *reinterpret_cast<u32*>(buf) = numLights;
            const size_t ssboHeader = 16;
            std::memcpy(
                buf + ssboHeader,
                runtimeScenePointLights.data(),
                bufSize - ssboHeader
            );
            sceneLightingBuffer.ensureLargeEnough(*renderer, bufSize);
            {
                un::BufferWriter writer(
                    renderer,
                    worker->getGraphicsResources().getCommandPool()
                );
                writer.overwrite(sceneLightingBuffer, buf);
            }
            lightsDirty = false;
            delete[] buf;
        }
        un::SceneLightingData sceneLightingData{};
        sceneLightingData.numPointLights = runtimeScenePointLights.size();
        sceneLightingData.pointLights = runtimeScenePointLights.data();
        un::BufferWriter objectBufferWriter(renderer);
        for (entt::entity entity : registry.view<un::ObjectLights, un::Translation>()) {
            auto& lights = registry.get<un::ObjectLights>(entity);
            glm::vec3 position = registry.get<un::Translation>(entity).value;
            std::vector<u32>& pointLights = lights.lights;
            std::vector<float> distances(
                maxNumLightsPerObject,
                std::numeric_limits<float>::max()
            );
            size_t objectNumLights = 0;
            if (pointLights.size() != maxNumLightsPerObject) {
                pointLights.resize(maxNumLightsPerObject);
            }
            for (size_t i = 0; i < runtimeScenePointLights.size(); i++) {
                un::PointLightData& candidate = runtimeScenePointLights[i];
                for (size_t j = maxNumLightsPerObject - 1; j >= 0; --j) {
                    un::PointLightData& holder = runtimeScenePointLights[pointLights[j]];
                    float toBeat = distances[j];
                    float attempt = glm::distance(
                        candidate.position,
                        position
                    );
                    if (attempt > toBeat) {
                        break;
                    }
                    distances[j] = attempt;
                    pointLights[j] = i;
                    if (objectNumLights < maxNumLightsPerObject) {
                        objectNumLights++;
                    }
                }
            }
            pointLights.resize(objectNumLights);
            un::ObjectLightingData data{};
            data.nPointLights = pointLights.size();
            std::memcpy(
                data.pointLightsIndices,
                pointLights.data(),
                sizeof(u32) * MAX_NUM_LIGHTS
            );
            objectBufferWriter.overwrite(
                lights.buffer,
                &data
            );
        }
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

}
//
// Created by brunorbsf on 19/12/2021.
//

#ifndef UNNECESSARYENGINE_PIPELINE_STAGE_H
#define UNNECESSARYENGINE_PIPELINE_STAGE_H

#include <filesystem>
#include <unnecessary/strings.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <vulkan/vulkan.hpp>

namespace un {
    class PipelineStage {
    private:
        vk::ShaderStageFlagBits stage;
        vk::ShaderModule module;
    public:
        PipelineStage(vk::ShaderStageFlagBits stage, const vk::ShaderModule& module);

        const vk::ShaderModule& getModule() const;

        vk::ShaderStageFlagBits getStageFlags() const;

        static PipelineStage fromBuffer(vk::Device device, const un::Buffer& buffer, vk::ShaderStageFlagBits flags) {
            size_t size = buffer.size();
            size_t rest = size % 4;
            if (rest != 0) {
                size += 4 - rest;
            }
            vk::ShaderModule vulkanModule = device.createShaderModule(
                vk::ShaderModuleCreateInfo(
                    (vk::ShaderModuleCreateFlags) 0,
                    size,
                    reinterpret_cast<const uint32_t*>(buffer.data())
                )
            );
            return un::PipelineStage(flags, vulkanModule);
        }

        static PipelineStage fromFile(
            vk::Device device,
            const std::filesystem::path& path,
            vk::ShaderStageFlagBits flags
        ) {
            un::Buffer spirv;
            un::files::read_file_into_buffer(path, spirv, std::ios::binary);
            return fromBuffer(device, spirv, flags);
        };
    };
}
#endif

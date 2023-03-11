//
// Created by brunorbsf on 19/12/2021.
//

#ifndef UNNECESSARYENGINE_PIPELINE_STAGE_H
#define UNNECESSARYENGINE_PIPELINE_STAGE_H

#include <filesystem>
#include <unnecessary/strings.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/file_jobs.h>
#include <vulkan/vulkan.hpp>

namespace un {
    class pipeline_stage {
    private:
        vk::ShaderStageFlagBits _stage;
        vk::ShaderModule module;

    public:
        pipeline_stage(
            vk::ShaderStageFlagBits stage, const vk::ShaderModule&
        module);

        const vk::ShaderModule& get_module() const;

        vk::ShaderStageFlagBits get_stage_flags() const;

        static pipeline_stage
        from_buffer(vk::Device device, const byte_buffer& buffer, vk::ShaderStageFlagBits flags) {
            size_t size = buffer.size();
            size_t rest = size % 4;
            if (rest != 0) {
                size += 4 - rest;
            }
            vk::ShaderModule vulkanModule = device.createShaderModule(
                vk::ShaderModuleCreateInfo(
                    static_cast<vk::ShaderModuleCreateFlags>(0),
                    size,
                    reinterpret_cast<const uint32_t*>(buffer.data())
                )
            );
            return pipeline_stage(flags, vulkanModule);
        }

        static pipeline_stage from_file(
            vk::Device device,
            const std::filesystem::path& path,
            vk::ShaderStageFlagBits flags
        ) {
            byte_buffer spirv;
            files::read_file_into_buffer(path, spirv, std::ios::binary);
            return from_buffer(device, spirv, flags);
        };
    };
}
#endif

//
// Created by brunorbsf on 19/12/2021.
//

#ifndef UNNECESSARYENGINE_PIPELINE_STAGE_H
#define UNNECESSARYENGINE_PIPELINE_STAGE_H

#include <filesystem>
#include <unnecessary/strings.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <vulkan/vulkan.hpp>

namespace un {
    class PipelineStage {
    private:
        vk::ShaderModule module;
    public:
        PipelineStage(const vk::ShaderModule& module);

        template<typename JobSystemType>
        static un::JobHandle loadFromPathAsync(
            un::JobChain<JobSystemType>& chain,
            vk::Device device,
            std::filesystem::path path,
            un::PipelineStage* result
        ) {
            un::Buffer buffer;
            un::JobHandle loadFile;
            chain.template immediately<un::LoadFileJob>(
                &loadFile,
                path,
                &buffer
            );
            un::JobHandle final;
            chain.template after<un::LambdaJob<>>(
                loadFile,
                &final,
                [&]() {

                    vk::ShaderModule vulkanModule = device.createShaderModule(
                        vk::ShaderModuleCreateInfo(
                            (vk::ShaderModuleCreateFlags) 0,
                            buffer.size(),
                            reinterpret_cast<const uint32_t*>(buffer.data())
                        )
                    );
                    *result = un::PipelineStage(vulkanModule);
                }
            );
        }
    };
}
#endif

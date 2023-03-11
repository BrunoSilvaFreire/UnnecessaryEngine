#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/sdf/generator/jobs/process_pixel_job.h>
#include <png++/image.hpp>

#ifndef UNNECESSARYENGINE_SDF_GENERATOR_H
#define UNNECESSARYENGINE_SDF_GENERATOR_H

namespace un::sdf {
    void process_sdf(
        int size,
        int nThreads,
        const std::string& imageFile,
        const std::string& outputFile,
        float min,
        float max
    );
}
#endif

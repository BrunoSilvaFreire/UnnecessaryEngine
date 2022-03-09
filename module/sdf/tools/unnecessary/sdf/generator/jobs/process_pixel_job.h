//
// Created by bruno on 17/01/2022.
//

#ifndef UNNECESSARYENGINE_PROCESS_PIXEL_JOB_H
#define UNNECESSARYENGINE_PROCESS_PIXEL_JOB_H

#include <unnecessary/jobs/parallel_for_job.h>
#include <png++/image.hpp>
#include <glm/glm.hpp>

namespace un {

    class ProcessPixelJob :public un::ParallelForJob<un::JobWorker> {
    private:
        png::image<png::rgba_pixel>* input;
        png::image<png::gray_pixel>* output;
        float minDistance;
        float maxDistance;

        glm::uvec2 outputToInputCoordinates(size_t x, size_t y);


    public:
        ProcessPixelJob(
            png::image<png::rgba_pixel>* input,
            png::image<png::gray_pixel>* output,
            float minDistance,
            float maxDistance
        );

        void operator()(size_t index, JobWorker* worker) override;

        bool isOutOfBounds(int nx, int ny, size_t width, size_t height);

        float computeDistanceOfPixel(const glm::uvec2& inputCoords);
    };
}
#endif
